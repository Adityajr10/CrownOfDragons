// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Ability/DragonAbility_MeteorSlam.h"
#include "AI/AbilityActor/DragonShockwaveActor.h"
#include "GameFramework/CharacterMovementComponent.h"

void UDragonAbility_MeteorSlam::Start(ADragonBaseAI* InOwner, AActor* InTarget)
{
	Super::Start(InOwner, InTarget);

	AbilityType = EDragonAbilityType::MeteorSlam;

	if (!OwnerDragon || !TargetActor)
	{
		bFinished = true;
		return;
	}

	Phase = ESlamPhase::Climb;
	PhaseTimer = 0.f;

	OwnerDragon->GetCharacterMovement()->StopMovementImmediately();
	OwnerDragon->GetCharacterMovement()->SetMovementMode(MOVE_Flying);
}

void UDragonAbility_MeteorSlam::Tick(float DeltaTime)
{
	if (!OwnerDragon) return;

	// Grounded recovery keeps ticking without a target; earlier phases end cleanly
	if (!TargetActor && Phase != ESlamPhase::Grounded)
	{
		OwnerDragon->GetCharacterMovement()->StopMovementImmediately();
		OwnerDragon->GetCharacterMovement()->SetMovementMode(MOVE_Flying);
		bFinished = true;
		return;
	}

	FVector DragonLoc = OwnerDragon->GetActorLocation();
	FVector PlayerLoc = TargetActor ? TargetActor->GetActorLocation() : DragonLoc;

	switch (Phase)
	{
	case ESlamPhase::Climb:
	{
		// Stay above the player while climbing
		FVector ClimbTarget = PlayerLoc + FVector::UpVector * ClimbHeight;
		FVector Dir = (ClimbTarget - DragonLoc).GetSafeNormal();

		OwnerDragon->SetActorLocation(DragonLoc + Dir * ClimbSpeed * DeltaTime);
		OwnerDragon->SetActorRotation(
			FRotator(0.f, Dir.Rotation().Yaw, 0.f));

		if (FVector::Dist(DragonLoc, ClimbTarget) < 300.f)
		{
			Phase = ESlamPhase::Hang;
			PhaseTimer = 0.f;

			OwnerDragon->GetCharacterMovement()->StopMovementImmediately();

			if (TelegraphMontage)
			{
				OwnerDragon->PlayAnimMontage(TelegraphMontage);
			}
		}
		break;
	}

	case ESlamPhase::Hang:
	{
		PhaseTimer += DeltaTime;

		// Face the player during the telegraph
		FRotator LookRot = (PlayerLoc - DragonLoc).Rotation();
		OwnerDragon->SetActorRotation(
			FMath::RInterpTo(OwnerDragon->GetActorRotation(), LookRot, DeltaTime, 4.f));

		if (PhaseTimer >= HangTime)
		{
			Phase = ESlamPhase::Slam;

			// Lead the target slightly
			FVector Predicted = PlayerLoc;
			if (APawn* PlayerPawn = Cast<APawn>(TargetActor))
			{
				float TimeToImpact =
					FVector::Dist(DragonLoc, PlayerLoc) / FMath::Max(SlamSpeed, 1.f);
				Predicted += PlayerPawn->GetVelocity() * TimeToImpact;
			}
			SlamTarget = Predicted;

			OwnerDragon->GetCharacterMovement()->DisableMovement();
			OwnerDragon->SetActorRotation(
				(SlamTarget - DragonLoc).GetSafeNormal().Rotation());
		}
		break;
	}

	case ESlamPhase::Slam:
	{
		FVector Dir = (SlamTarget - DragonLoc).GetSafeNormal();
		FVector NewLoc = DragonLoc + Dir * SlamSpeed * DeltaTime;

		FHitResult MoveHit;
		OwnerDragon->SetActorLocation(NewLoc, true, &MoveHit);

		// Hitting the ground early counts as the impact
		if (MoveHit.bBlockingHit ||
			FVector::Dist(OwnerDragon->GetActorLocation(), SlamTarget) < 250.f)
		{
			DoImpact();
		}
		break;
	}

	case ESlamPhase::Grounded:
	{
		PhaseTimer += DeltaTime;

		if (PhaseTimer >= GroundedTime)
		{
			ExitGrounded();
			bFinished = true;
		}
		break;
	}
	}
}

void UDragonAbility_MeteorSlam::DoImpact()
{
	Phase = ESlamPhase::Grounded;
	PhaseTimer = 0.f;

	OwnerDragon->GetCharacterMovement()->StopMovementImmediately();
	OwnerDragon->DisableFlyingMode();

	if (ImpactMontage)
	{
		OwnerDragon->PlayAnimMontage(ImpactMontage);
	}

	if (ShockwaveClass)
	{
		FActorSpawnParameters Params;
		Params.Owner = OwnerDragon;

		OwnerDragon->GetWorld()->SpawnActor<ADragonShockwaveActor>(
			ShockwaveClass,
			OwnerDragon->GetActorLocation(),
			FRotator::ZeroRotator,
			Params);
	}
}

void UDragonAbility_MeteorSlam::ExitGrounded()
{
	OwnerDragon->GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	OwnerDragon->EnableFlyingMode();
}

bool UDragonAbility_MeteorSlam::IsFinished() const
{
	return bFinished;
}

// Climb/Hang: yes — not committed yet
// Slam: NO — mid-crash
// Grounded: yes — this is the punish window
bool UDragonAbility_MeteorSlam::CanBeInterrupted() const
{
	return Phase != ESlamPhase::Slam;
}

void UDragonAbility_MeteorSlam::Abort(EDragonInterruptReason Reason)
{
	if (OwnerDragon)
	{
		OwnerDragon->GetCharacterMovement()->StopMovementImmediately();
		OwnerDragon->GetCharacterMovement()->SetMovementMode(MOVE_Flying);

		if (Phase == ESlamPhase::Grounded)
		{
			OwnerDragon->EnableFlyingMode();
		}
	}

	Super::Abort(Reason);
}
