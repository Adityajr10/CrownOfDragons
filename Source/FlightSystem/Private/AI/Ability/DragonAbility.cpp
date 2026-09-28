#include "AI/Ability/DragonAbility.h"

void UDragonAbility::Start(ADragonBaseAI* InOwner, AActor* InTarget)
{
	OwnerDragon = InOwner;
	TargetActor = InTarget;
	bFinished = false;
	bAborted = false;
	SpawnedCount = 0;
}

void UDragonAbility::Tick(float DeltaTime)
{
	// Base does nothing — subclasses implement
}

bool UDragonAbility::IsFinished() const
{
	return bFinished;
}

bool UDragonAbility::CanBeInterrupted() const
{
	return true;
}

void UDragonAbility::Abort(EDragonInterruptReason Reason)
{
	UE_LOG(LogTemp, Warning, TEXT("Ability [%s] aborted — reason: %s"),
		*UEnum::GetValueAsString(AbilityType),
		*UEnum::GetValueAsString(Reason));

	bAborted = true;
	bFinished = true;

	// Subclasses override to:
	// 1. Stop montages: OwnerDragon->StopAnimMontage(...)
	// 2. Destroy spawned actors
	// 3. Reset movement modes
	// Then call Super::Abort(Reason)
}