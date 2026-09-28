#include "AI/Component/DragonAbilityComponent.h"
#include "AI/Ability/DragonAbility.h"
#include "AI/Component/DragonAIBehaviourComponent.h"
#include "AI/Component/DragonStimulusComponent.h"
#include "FlightSystem/DragonBaseAI.h"

UDragonAbilityComponent::UDragonAbilityComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UDragonAbilityComponent::BeginPlay()
{
	Super::BeginPlay();
	OwnerDragon = Cast<ADragonBaseAI>(GetOwner());

	// Bind to stimulus component
	if (OwnerDragon)
	{
		UDragonStimulusComponent* Stimulus =
			OwnerDragon->FindComponentByClass<UDragonStimulusComponent>();

		if (Stimulus)
		{
			Stimulus->OnStimulusReceived.AddDynamic(
				this, &UDragonAbilityComponent::OnStimulusWhileAbilityActive);
		}
	}
}

void UDragonAbilityComponent::StartAbility(UDragonAbility* Ability, AActor* Target)
{
	if (!Ability || ActiveAbility) return;

	ActiveAbility = Ability;
	AbilityRunningTime = 0.f;

	// Start first so AbilityType gets set in the subclass
	ActiveAbility->Start(OwnerDragon, Target);

	// Cache data
	CachedAbortDistanceThresholdSq = FLT_MAX;
	bCachedCanBeInterrupted = true;
	CachedForceInterruptHealthPercent = 0.f;

	if (OwnerDragon)
	{
		UDragonAIBehaviourComponent* Behaviour = GetBehaviour();
		if (Behaviour)
		{
			Behaviour->OnAbilityStarted();

			const FDragonAbilityData* AbilityData =
				Behaviour->FindAbilityData(ActiveAbility->AbilityType);

			if (AbilityData)
			{
				CachedAbortDistanceThresholdSq =
					AbilityData->AbortDistanceThreshold * AbilityData->AbortDistanceThreshold;

				bCachedCanBeInterrupted = AbilityData->bCanBeInterrupted;
				CachedForceInterruptHealthPercent = AbilityData->ForceInterruptHealthPercent;

				UE_LOG(LogTemp, Warning,
					TEXT("Ability [%s] started — AbortDist=%.0f Interruptible=%s"),
					*UEnum::GetValueAsString(ActiveAbility->AbilityType),
					AbilityData->AbortDistanceThreshold,
					bCachedCanBeInterrupted ? TEXT("Yes") : TEXT("No"));
			}
		}
	}
}

void UDragonAbilityComponent::TickComponent(
	float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!ActiveAbility) return;

	AbilityRunningTime += DeltaTime;

	// Exit 1: Hard timeout
	if (AbilityRunningTime >= HardAbilityDuration)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("Ability HARD timeout after %.1fs"), AbilityRunningTime);
		FinishAbility(EDragonInterruptReason::Timeout);
		return;
	}

	// Exit 2: Soft timeout + distance check
	if (AbilityRunningTime >= MaxAbilityDuration)
	{
		UDragonAIBehaviourComponent* Behaviour = GetBehaviour();

		if (Behaviour && Behaviour->TargetActor)
		{
			float DistSq = FVector::DistSquared(
				OwnerDragon->GetActorLocation(),
				Behaviour->TargetActor->GetActorLocation());

			if (DistSq > CachedAbortDistanceThresholdSq)
			{
				UE_LOG(LogTemp, Warning,
					TEXT("Ability soft-timeout: target out of range after %.1fs"),
					AbilityRunningTime);
				FinishAbility(EDragonInterruptReason::OutOfRange);
				return;
			}
		}
		else if (!Behaviour || !Behaviour->TargetActor)
		{
			UE_LOG(LogTemp, Warning,
				TEXT("Ability soft-timeout: no target after %.1fs"),
				AbilityRunningTime);
			FinishAbility(EDragonInterruptReason::Timeout);
			return;
		}
	}

	// Normal tick
	ActiveAbility->Tick(DeltaTime);

	// Exit 3: Natural finish
	if (ActiveAbility && ActiveAbility->IsFinished())
	{
		UE_LOG(LogTemp, Log, TEXT("Ability finished naturally"));
		FinishAbility();
	}
}

// ============================================================
// Interrupt system
// ============================================================

bool UDragonAbilityComponent::RequestInterrupt(EDragonInterruptReason Reason)
{
	if (!ActiveAbility) return false;

	bool bAbilityAllows = ActiveAbility->CanBeInterrupted();
	bool bDataAllows = bCachedCanBeInterrupted;

	// Health override — even uninterruptible abilities break at critical health
	bool bHealthForce = false;
	if (CachedForceInterruptHealthPercent > 0.f && OwnerDragon)
	{
		UDragonStimulusComponent* Stimulus =
			OwnerDragon->FindComponentByClass<UDragonStimulusComponent>();

		if (Stimulus && Stimulus->HealthPercent <= CachedForceInterruptHealthPercent)
		{
			bHealthForce = true;
		}
	}

	if ((bAbilityAllows && bDataAllows) || bHealthForce)
	{
		UE_LOG(LogTemp, Warning, TEXT("interruptible Ability interrupted — reason: %s (health_force=%s)"),
			*UEnum::GetValueAsString(Reason),
			bHealthForce ? TEXT("Yes") : TEXT("No"));

		ActiveAbility->Abort(Reason);
		FinishAbility(Reason);
		return true;
	}

	UE_LOG(LogTemp, Log,
		TEXT("interruptible Interrupt denied — ability [%s] not interruptible right now"),
		*UEnum::GetValueAsString(ActiveAbility->AbilityType));
	return false;
}

void UDragonAbilityComponent::ForceInterrupt()
{
	if (!ActiveAbility) return;
	ActiveAbility->Abort(EDragonInterruptReason::Forced);
	FinishAbility(EDragonInterruptReason::Forced);
}

void UDragonAbilityComponent::OnStimulusWhileAbilityActive(const FDragonStimulusInfo& Stimulus)
{
	if (!ActiveAbility) return;

	// Forward damage info to the ability for flinch animations
	if (Stimulus.Type == EDragonStimulus::DamageTaken ||
		Stimulus.Type == EDragonStimulus::HeavyDamageTaken)
	{
		ActiveAbility->OnOwnerDamaged(Stimulus.Value, Stimulus.Value, Stimulus.Source);
	}

	// The brain (BehaviourComponent) decides whether to actually interrupt.
	// It calls RequestInterrupt() on us if needed.
}

/*
void UDragonAbilityComponent::FinishAbility(EDragonInterruptReason Reason)
{
	ActiveAbility = nullptr;
	AbilityRunningTime = 0.f;

	UDragonAIBehaviourComponent* Behaviour = GetBehaviour();
	if (Behaviour)
	{
		Behaviour->OnAbilityFinished();
	}
}
*/

void UDragonAbilityComponent::FinishAbility(EDragonInterruptReason Reason)
{
	// Timeout finishes bypass RequestInterrupt, so run the ability's cleanup
	// here — otherwise movement modes / visibility set mid-phase leak
	if (ActiveAbility &&
		(Reason == EDragonInterruptReason::Timeout ||
		 Reason == EDragonInterruptReason::OutOfRange) &&
		!ActiveAbility->WasAborted() &&
		!ActiveAbility->IsFinished())
	{
		ActiveAbility->Abort(Reason);
	}

	ActiveAbility = nullptr;
	AbilityRunningTime = 0.f;
	LastFinishReason = Reason;

	UDragonAIBehaviourComponent* Behaviour = GetBehaviour();
	if (Behaviour)
	{
		// Only do immediate selection if ability finished naturally
		if (Reason == EDragonInterruptReason::None ||
			Reason == EDragonInterruptReason::Timeout ||
			Reason == EDragonInterruptReason::OutOfRange)
		{
			Behaviour->OnAbilityFinished();
		}
		// For damage interrupts, just update blackboard
		// The BT task will see IsAbilityActive()==false and finish
		// Then OnAbilityFinished runs from a delayed call
		else
		{
			Behaviour->UpdateBlackboard();

			GetWorld()->GetTimerManager().SetTimerForNextTick([Behaviour]()
			{
				if (Behaviour)
				{
					Behaviour->OnAbilityFinished();
				}
			});
		}
	}
}

UDragonAIBehaviourComponent* UDragonAbilityComponent::GetBehaviour() const
{
	if (!CachedBehaviour && OwnerDragon)
	{
		CachedBehaviour = OwnerDragon->FindComponentByClass<UDragonAIBehaviourComponent>();
	}
	return CachedBehaviour;
}

bool UDragonAbilityComponent::IsAbilityActive() const
{
	return ActiveAbility != nullptr;
}

EDragonAbilityType UDragonAbilityComponent::GetActiveAbilityType() const
{
	return ActiveAbility ? ActiveAbility->AbilityType : EDragonAbilityType::None;
}