// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Ability/DragonAbility_DiveBomb.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

void UDragonAbility_DiveBomb::Start(ADragonBaseAI* InOwner, AActor* InTarget)
{
	Super::Start(InOwner, InTarget);

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

	// PHASE 1: reach crest
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
