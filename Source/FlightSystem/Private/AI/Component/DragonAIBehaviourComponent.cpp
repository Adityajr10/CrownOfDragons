// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Component/DragonAIBehaviourComponent.h"

UDragonAIBehaviourComponent::UDragonAIBehaviourComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UDragonAIBehaviourComponent::BeginPlay()
{
	Super::BeginPlay();

	HomeLocation = GetOwner()->GetActorLocation();
	CurrentEnergy = MaxEnergy;

	OwnerAIController = Cast<AAIController>(GetOwner()->GetInstigatorController());

	if (OwnerAIController)
	{
		BlackboardComponent = OwnerAIController->GetBlackboardComponent();
	}
}

bool UDragonAIBehaviourComponent::CanUseAbility(const FDragonAbilityData& AbilityData) const
{
	// Check Energy 
	if (CurrentEnergy < AbilityData.EnergyCost)
	{
		return false;
	}
	//Check Cooldown 
	const float* LastTime = LastAbilityUseTime.Find(AbilityData.AbilityType);

	if (LastTime)
	{
		float CurrentTime = GetWorld()->GetTimeSeconds();

		if (CurrentTime - *LastTime < AbilityData.Cooldown)
		{
			return false;
		}
	}

	return true;
}

void UDragonAIBehaviourComponent::ConsumeEnergy(float Amount)
{
	CurrentEnergy -= Amount;

	CurrentEnergy = FMath::Clamp(CurrentEnergy, 0.f, MaxEnergy);
}

void UDragonAIBehaviourComponent::RegenerateEnergy(float DeltaTime)
{
	CurrentEnergy += EnergyRegenRate * DeltaTime;

	CurrentEnergy = FMath::Clamp(CurrentEnergy, 0.f, MaxEnergy);
}

void UDragonAIBehaviourComponent::UseAbility(const FDragonAbilityData& AbilityData)
{
	float CurrentTime = GetWorld()->GetTimeSeconds();

	LastAbilityUseTime.Add(AbilityData.AbilityType, CurrentTime);

	ConsumeEnergy(AbilityData.EnergyCost);

	SecondLastUsedAbility = LastUsedAbility;
	LastUsedAbility = AbilityData.AbilityType;

	CurrentAbility = AbilityData.AbilityType;

	//Set next attack time 

	float Delay = FMath::FRandRange(MinAttackInterval, MaxAttackInterval);

	NextAttackTime = CurrentTime + Delay;
}

void UDragonAIBehaviourComponent::EvaluateSituation()
{
	if (!GetWorld())
	{
		return;
	}
	float CurrentTime = GetWorld()->GetTimeSeconds();

	//Territory Check 

	float DistanceFromHome = GetDistanceFromHome();

	if (DistanceFromHome > MaxChaseDistance)
	{
		TargetActor = nullptr;
	}
	//Target Memory Update 
	if (TargetActor)
	{
		LastKnownTargetLocation = TargetActor->GetActorLocation();
		LastSeenTime = CurrentTime;
	}
	else
	{
		if (CurrentTime - LastSeenTime > TargetMemoryDuration)
		{
			LastKnownTargetLocation = FVector::ZeroVector;
		}
	}
	if (CurrentTime >= NextInstinctChangeTime)
	{
		SelectInstinct();

		float Duration = FMath::FRandRange(MinInstinctDuration, MaxInstinctDuration);
		NextInstinctChangeTime = CurrentTime + Duration;
	}
	SelectAbility();
	UpdateBlackboard();
}

void UDragonAIBehaviourComponent::SelectInstinct()
{
	float Distance = GetDistanceToTarget();

	float RoamingScore = 0.f;
	float StalkingScore = 0.f;
	float ThreatScore = 0.f;
	float AttackScore = 0.f;
	float RestScore = 0.f;

	// No target 
	if (!TargetActor)
	{
		RoamingScore += 1.0f;
	}

	else
	{
		// Distance influence 
		if (Distance > 6000.f)
		{
			StalkingScore += 0.8f;
		}
		else if (Distance > 2000.f)
		{
			ThreatScore += 0.7f;
		}
		else
		{
			AttackScore += 0.8f;
		}

		//Personality influence
		AttackScore += PersonalitySettings.Aggression * 0.5f;

		StalkingScore += PersonalitySettings.Patience * 0.3f;

		ThreatScore += PersonalitySettings.Intelligence * 0.2f;
	}

	//Energy influence
	if (CurrentEnergy < 20.f)
	{
		RestScore += 1.0f;
	}

	//Find highest score 
	float BestScore = RoamingScore;
	CurrentInstinct = EDragonInstinct::Roaming;

	if (StalkingScore > BestScore)
	{
		BestScore = StalkingScore;
		CurrentInstinct = EDragonInstinct::Stalking;
	}

	if (ThreatScore > BestScore)
	{
		BestScore = ThreatScore;
		CurrentInstinct = EDragonInstinct::Threatening;
	}

	if (AttackScore > BestScore)
	{
		BestScore = AttackScore;
		CurrentInstinct = EDragonInstinct::Attacking;
	}

	if (RestScore > BestScore)
	{
		BestScore = RestScore;
		CurrentInstinct = EDragonInstinct::Resting;
	}

	// Update State
	switch (CurrentInstinct)
	{
	case EDragonInstinct::Roaming:
		CurrentState = EDragonState::Roaming;
		break;

	case EDragonInstinct::Stalking:
	case EDragonInstinct::Threatening:
		CurrentState = EDragonState::Observe;
		break;

	case EDragonInstinct::Attacking:
		CurrentState = EDragonState::Attack;
		break;

	case EDragonInstinct::Resting:
		CurrentState = EDragonState::Rest;
		break;
	}
}

void UDragonAIBehaviourComponent::SelectAbility()
{
	if (!TargetActor)
	{
		return;
	}

	if (!CanPerformAttack())
	{
		return;
	}

	float DistanceToTarget = GetDistanceToTarget();
	float AltitudeDiff = GetTargetAltitudeDifference();   // ← ADD HERE

	float BestScore = -1.f;
	const FDragonAbilityData* BestAbility = nullptr;

	for (const FDragonAbilityData& Ability : AbilitySettings)
	{
		if (Ability.InstinctType != CurrentInstinct)
		{
			continue;
		}

		if (!CanUseAbility(Ability))
		{
			continue;
		}

		// Distance check
		if (DistanceToTarget < Ability.PreferredDistanceMin ||
			DistanceToTarget > Ability.PreferredDistanceMax)
		{
			continue;
		}

		// Altitude check (example rule)
		if (AltitudeDiff < Ability.MinAltitudeDifference ||
	    AltitudeDiff > Ability.MaxAltitudeDifference)
		{
			continue;
		}

		// Base score
		float RandomFactor = FMath::FRandRange(0.8f, 1.2f);
		float Score = Ability.Weight * RandomFactor;

		// Ability repetition penalty
		if (Ability.AbilityType == LastUsedAbility)
		{
			Score *= 0.7f;
		}

		if (Ability.AbilityType == SecondLastUsedAbility)
		{
			Score *= 0.85f;
		}

		// Select best ability
		if (Score > BestScore)
		{
			BestScore = Score;
			BestAbility = &Ability;
		}
	}

	if (BestAbility)
	{
		UseAbility(*BestAbility);
	}
}

float UDragonAIBehaviourComponent::GetDistanceToTarget() const
{
	if (!TargetActor)
	{
		return -1.f;
	}

	return FVector::Dist(
		GetOwner()->GetActorLocation(),
		TargetActor->GetActorLocation()
	);
}

void UDragonAIBehaviourComponent::UpdateBlackboard()
{
	if (!BlackboardComponent)
	{
		return;
	}

	BlackboardComponent->SetValueAsEnum(
		TEXT("CurrentState"),
		static_cast<uint8>(CurrentState)
	);

	BlackboardComponent->SetValueAsEnum(
		TEXT("CurrentAbility"),
		static_cast<uint8>(CurrentAbility)
	);

	BlackboardComponent->SetValueAsObject(
		TEXT("TargetActor"),
		TargetActor
	);
	BlackboardComponent->SetValueAsVector(
	TEXT("LastKnownTargetLocation"),
	LastKnownTargetLocation
);
}

bool UDragonAIBehaviourComponent::CanPerformAttack() const
{
	if (!GetWorld())
	{
		return false;
	}

	float CurrentTime = GetWorld()->GetTimeSeconds();

	return CurrentTime >= NextAttackTime;
}

void UDragonAIBehaviourComponent::SetTargetActor(AActor* NewTarget)
{
	TargetActor = NewTarget;

	if (NewTarget)
	{
		LastKnownTargetLocation = NewTarget->GetActorLocation();
		LastSeenTime = GetWorld()->GetTimeSeconds();
	}
}

bool UDragonAIBehaviourComponent::IsValidTarget(AActor* Actor) const
{
	if (!Actor)
	{
		return false;
	}

	if (Actor == GetOwner())
	{
		return false;
	}

	if (Actor->IsActorBeingDestroyed())
	{
		return false;
	}

	return true;
}

float UDragonAIBehaviourComponent::GetTargetAltitudeDifference() const
{
	if (!TargetActor)
	{
		return 0.f;
	}

	float DragonZ = GetOwner()->GetActorLocation().Z;
	float TargetZ = TargetActor->GetActorLocation().Z;

	return DragonZ - TargetZ;
}

FVector UDragonAIBehaviourComponent::PredictTargetLocation(float PredictionTime) const
{
	if (!TargetActor)
	{
		return FVector::ZeroVector;
	}

	FVector TargetLocation = TargetActor->GetActorLocation();

	APawn* TargetPawn = Cast<APawn>(TargetActor);

	if (!TargetPawn)
	{
		return TargetLocation;
	}

	FVector Velocity = TargetPawn->GetVelocity();

	return TargetLocation + Velocity * PredictionTime;
}

float UDragonAIBehaviourComponent::GetDistanceFromHome() const
{
	return FVector::Dist(GetOwner()->GetActorLocation(), HomeLocation);
}