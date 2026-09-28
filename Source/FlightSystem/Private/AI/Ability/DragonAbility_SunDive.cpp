// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Ability/DragonAbility_SunDive.h"
#include "AI/AbilityActor/DragonShockwaveActor.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

void UDragonAbility_SunDive::Start(ADragonBaseAI* InOwner, AActor* InTarget)
{
	Super::Start(InOwner, InTarget);

	AbilityType = EDragonAbilityType::SunDive;

	if (!OwnerDragon || !TargetActor)
	{
		bFinished = true;
		return;
	}

	Phase = ESunDivePhase::Climb;
	PhaseTimer = 0.f;
	VanishTime = FMath::FRandRange(VanishTimeMin, VanishTimeMax);

	OwnerDragon->GetCharacterMovement()->StopMovementImmediately();
	OwnerDragon->GetCharacterMovement()->SetMovementMode(MOVE_Flying);
}

void UDragonAbility_SunDive::Tick(float DeltaTime)
{
	if (!OwnerDragon) return;

	// The dive is locked to DiveTarget and keeps going without a target;
	// losing the target in climb/vanish ends cleanly (and always unhides)
	if (!TargetActor && Phase != ESunDivePhase::Dive)
	{
		SetDragonHidden(false);
		OwnerDragon->GetCharacterMovement()->StopMovementImmediately();
		OwnerDragon->GetCharacterMovement()->SetMovementMode(MOVE_Flying);
		bFinished = true;
		return;
	}

	FVector DragonLoc = OwnerDragon->GetActorLocation();
	FVector PlayerLoc = TargetActor ? TargetActor->GetActorLocation() : DragonLoc;

	switch (Phase)
	{
	case ESunDivePhase::Climb:
	{
		FVector ClimbTarget = PlayerLoc + FVector::UpVector * VanishHeight;
		FVector Dir = (ClimbTarget - DragonLoc).GetSafeNormal();

		OwnerDragon->SetActorLocation(DragonLoc + Dir * ClimbSpeed * DeltaTime);
		OwnerDragon->SetActorRotation(Dir.Rotation());

		if (FVector::Dist(DragonLoc, ClimbTarget) < 500.f)
		{
			Phase = ESunDivePhase::Vanish;
			PhaseTimer = 0.f;

			if (bHideDuringVanish)
			{
				SetDragonHidden(true);
			}

			if (WingbeatSound)
			{
				UGameplayStatics::PlaySoundAtLocation(
					OwnerDragon, WingbeatSound, PlayerLoc + FVector(0, 0, 1500.f));
			}
		}
		break;
	}

	case ESunDivePhase::Vanish:
	{
		PhaseTimer += DeltaTime;

		// Silently shadow the player's XY from altitude
		FVector ShadowTarget = PlayerLoc + FVector::UpVector * VanishHeight;
		FVector Dir = (ShadowTarget - DragonLoc).GetSafeNormal();
		OwnerDragon->SetActorLocation(
			DragonLoc + Dir * ShadowSpeed * DeltaTime);

		if (PhaseTimer >= VanishTime)
		{
			Phase = ESunDivePhase::Dive;

			SetDragonHidden(false);

			FVector Predicted = PlayerLoc;
			if (APawn* PlayerPawn = Cast<APawn>(TargetActor))
			{
				float TimeToImpact =
					FVector::Dist(DragonLoc, PlayerLoc) / FMath::Max(DiveSpeed, 1.f);
				Predicted += PlayerPawn->GetVelocity() * TimeToImpact;
			}
			DiveTarget = Predicted;

			OwnerDragon->GetCharacterMovement()->DisableMovement();
			OwnerDragon->SetActorRotation(
				(DiveTarget - DragonLoc).GetSafeNormal().Rotation());

			if (DiveSound)
			{
				UGameplayStatics::PlaySoundAtLocation(
					OwnerDragon, DiveSound, PlayerLoc + FVector(0, 0, 2000.f));
			}
		}
		break;
	}

	case ESunDivePhase::Dive:
	{
		FVector Dir = (DiveTarget - DragonLoc).GetSafeNormal();
		FVector NewLoc = DragonLoc + Dir * DiveSpeed * DeltaTime;

		FHitResult MoveHit;
		OwnerDragon->SetActorLocation(NewLoc, true, &MoveHit);

		if (MoveHit.bBlockingHit ||
			FVector::Dist(OwnerDragon->GetActorLocation(), DiveTarget) < 300.f)
		{
			if (ShockwaveClass)
			{
				FActorSpawnParameters Params;
				Params.Owner = OwnerDragon;

				OwnerDragon->GetWorld()->SpawnActor<ADragonShockwaveActor>(
					ShockwaveClass, OwnerDragon->GetActorLocation(),
					FRotator::ZeroRotator, Params);
			}

			if (ImpactMontage)
			{
				OwnerDragon->PlayAnimMontage(ImpactMontage);
			}

			OwnerDragon->GetCharacterMovement()->SetMovementMode(MOVE_Flying);
			bFinished = true;
		}
		break;
	}
	}
}

void UDragonAbility_SunDive::SetDragonHidden(bool bHidden)
{
	if (OwnerDragon)
	{
		OwnerDragon->SetActorHiddenInGame(bHidden);
	}
}

bool UDragonAbility_SunDive::IsFinished() const
{
	return bFinished;
}

// Only the climb can be interrupted — once vanished, it's committed
bool UDragonAbility_SunDive::CanBeInterrupted() const
{
	return Phase == ESunDivePhase::Climb;
}

void UDragonAbility_SunDive::Abort(EDragonInterruptReason Reason)
{
	if (OwnerDragon)
	{
		// Never leave the dragon invisible or frozen
		SetDragonHidden(false);
		OwnerDragon->GetCharacterMovement()->StopMovementImmediately();
		OwnerDragon->GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	}

	Super::Abort(Reason);
}
