#include "AI/Ability/DragonAbility_DiveBomb.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

void UDragonAbility_DiveBomb::Start(ADragonBaseAI* InOwner, AActor* InTarget)
{
	Super::Start(InOwner, InTarget);

	AbilityType = EDragonAbilityType::DiveBomb;
	if (!OwnerDragon || !TargetActor)
	{
		bFinished = true;
		return;
	}

	bReachedTop = false;

	FVector ToPlayer = (TargetActor->GetActorLocation() - OwnerDragon->GetActorLocation()).GetSafeNormal();
	FVector ClimbDir = (ToPlayer + FVector::UpVector * 1.2f).GetSafeNormal();

	OwnerDragon->SetActorRotation(ClimbDir.Rotation());
	OwnerDragon->LaunchCharacter(ClimbDir * 4000.f, true, true);
}

void UDragonAbility_DiveBomb::Tick(float DeltaTime)
{
	if (!OwnerDragon || !TargetActor) return;

	// PHASE 1: reach crest (climbing)
	if (!bReachedTop)
	{
		if (OwnerDragon->GetVelocity().Z < 50.f)
		{
			bReachedTop = true;

			OwnerDragon->GetCharacterMovement()->StopMovementImmediately();
			OwnerDragon->GetCharacterMovement()->DisableMovement();

			FVector DiveDir = (TargetActor->GetActorLocation() - OwnerDragon->GetActorLocation()).GetSafeNormal();
			OwnerDragon->SetActorRotation(DiveDir.Rotation());
		}

		return;
	}

	// PHASE 2: homing dive
	FVector CurrentLoc = OwnerDragon->GetActorLocation();
	FVector TargetLoc = TargetActor->GetActorLocation();

	FVector Dir = (TargetLoc - CurrentLoc).GetSafeNormal();
	float Step = OwnerDragon->DiveSpeed * DeltaTime;

	FVector NewLoc = CurrentLoc + Dir * Step;

	OwnerDragon->SetActorLocation(NewLoc);
	OwnerDragon->SetActorRotation(Dir.Rotation());

	if (FVector::Dist(NewLoc, TargetLoc) < 200.f)
	{
		OwnerDragon->OnDiveImpact();

		OwnerDragon->GetCharacterMovement()->SetMovementMode(MOVE_Flying);

		bFinished = true;
	}
}

bool UDragonAbility_DiveBomb::IsFinished() const
{
	return bFinished;
}

// Phase 1 (climbing up): YES — can abort before committing to the dive
// Phase 2 (diving down): NO — mid-dive, can't pull out
bool UDragonAbility_DiveBomb::CanBeInterrupted() const
{
	return !bReachedTop;
}

void UDragonAbility_DiveBomb::Abort(EDragonInterruptReason Reason)
{
	if (OwnerDragon)
	{
		// If aborted during dive (force-interrupt via health), re-enable movement
		// so the dragon doesn't stay frozen with movement disabled
		if (bReachedTop)
		{
			OwnerDragon->GetCharacterMovement()->SetMovementMode(MOVE_Flying);
			UE_LOG(LogTemp, Log, TEXT("DiveBomb: force-aborted during dive — re-enabled movement"));
		}
		else
		{
			// Aborted during climb — stop the launch velocity
			OwnerDragon->GetCharacterMovement()->StopMovementImmediately();
			UE_LOG(LogTemp, Log, TEXT("DiveBomb: aborted during climb"));
		}
	}

	Super::Abort(Reason);
}