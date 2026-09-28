// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Ability/DragonAbility_CarpetBomb.h"
#include "AI/AbilityActor/DragonBombActor.h"
#include "GameFramework/CharacterMovementComponent.h"

void UDragonAbility_CarpetBomb::Start(ADragonBaseAI* InOwner, AActor* InTarget)
{
	Super::Start(InOwner, InTarget);

	AbilityType = EDragonAbilityType::CarpetBomb;

	if (!OwnerDragon || !TargetActor)
	{
		bFinished = true;
		return;
	}

	FVector DragonLoc = OwnerDragon->GetActorLocation();
	FVector PlayerLoc = TargetActor->GetActorLocation();

	// Horizontal run line through the player, entered from the dragon's side
	RunDir = (PlayerLoc - DragonLoc).GetSafeNormal2D();
	if (RunDir.IsNearlyZero())
	{
		RunDir = OwnerDragon->GetActorForwardVector().GetSafeNormal2D();
	}

	float RunAltitude = PlayerLoc.Z + BombAltitude;

	RunStart = PlayerLoc - RunDir * RunLeadDistance;
	RunStart.Z = RunAltitude;

	RunEnd = PlayerLoc + RunDir * RunOvershoot;
	RunEnd.Z = RunAltitude;

	DropSpacing = (RunLeadDistance + RunOvershoot) / FMath::Max(BombCount, 1);

	bRunStarted = false;
	BombsDropped = 0;

	OwnerDragon->GetCharacterMovement()->StopMovementImmediately();
	OwnerDragon->GetCharacterMovement()->SetMovementMode(MOVE_Flying);
}

void UDragonAbility_CarpetBomb::Tick(float DeltaTime)
{
	if (!OwnerDragon) return;

	// The run line is fixed at Start — only the approach needs a live target
	if (!TargetActor && !bRunStarted)
	{
		bFinished = true;
		return;
	}

	FVector DragonLoc = OwnerDragon->GetActorLocation();

	// Phase 1: fly to the start of the run
	if (!bRunStarted)
	{
		FVector Dir = (RunStart - DragonLoc).GetSafeNormal();
		OwnerDragon->SetActorLocation(DragonLoc + Dir * RunSpeed * DeltaTime);
		OwnerDragon->SetActorRotation(
			FRotator(0.f, Dir.Rotation().Yaw, 0.f));

		if (FVector::Dist(DragonLoc, RunStart) < 300.f)
		{
			bRunStarted = true;
			OwnerDragon->SetActorRotation(
				FRotator(0.f, RunDir.Rotation().Yaw, 0.f));

			if (BombRunMontage)
			{
				OwnerDragon->PlayAnimMontage(BombRunMontage);
			}
		}
		return;
	}

	// Phase 2: straight bombing run
	FVector Dir = (RunEnd - DragonLoc).GetSafeNormal();
	FVector NewLoc = DragonLoc + Dir * RunSpeed * DeltaTime;
	OwnerDragon->SetActorLocation(NewLoc);

	float TraveledFromStart = FVector::Dist2D(NewLoc, RunStart);

	if (BombsDropped < BombCount &&
		TraveledFromStart >= DropSpacing * (BombsDropped + 1))
	{
		DropBomb();
	}

	if (FVector::Dist(NewLoc, RunEnd) < 300.f)
	{
		bFinished = true;
	}
}

void UDragonAbility_CarpetBomb::DropBomb()
{
	BombsDropped++;

	if (!BombClass) return;

	UWorld* World = OwnerDragon->GetWorld();
	if (!World) return;

	FActorSpawnParameters Params;
	Params.Owner = OwnerDragon;

	ADragonBombActor* Bomb = World->SpawnActor<ADragonBombActor>(
		BombClass,
		OwnerDragon->GetActorLocation() - FVector(0.f, 0.f, 200.f),
		FRotator::ZeroRotator,
		Params);

	if (Bomb)
	{
		// Bombs inherit part of the run velocity and arc forward
		Bomb->Init(RunDir * RunSpeed * 0.5f);
	}
}

bool UDragonAbility_CarpetBomb::IsFinished() const
{
	return bFinished;
}

// Approach: yes — Run: no, committed to the pass
bool UDragonAbility_CarpetBomb::CanBeInterrupted() const
{
	return !bRunStarted;
}

void UDragonAbility_CarpetBomb::Abort(EDragonInterruptReason Reason)
{
	if (OwnerDragon)
	{
		OwnerDragon->GetCharacterMovement()->StopMovementImmediately();
		OwnerDragon->GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	}

	Super::Abort(Reason);
}
