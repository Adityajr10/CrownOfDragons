// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Ability/DragonAbility_Cyclone.h"
#include "AI/AbilityActor/DragonCycloneActor.h"

void UDragonAbility_Cyclone::Start(ADragonBaseAI* InOwner, AActor* InTarget)
{
	Super::Start(InOwner, InTarget);

	AbilityType = EDragonAbilityType::Cyclone;

	bChanneling = false;
	bSpawned = false;
	Timer = 0.f;

	if (!OwnerDragon || !TargetActor)
	{
		bFinished = true;
	}
}

void UDragonAbility_Cyclone::Tick(float DeltaTime)
{
	if (!OwnerDragon) return;

	if (!TargetActor)
	{
		bFinished = true;
		return;
	}

	UDragonFlightComponent* Flight =
		OwnerDragon->FindComponentByClass<UDragonFlightComponent>();

	if (!Flight) return;

	FVector DragonLoc = OwnerDragon->GetActorLocation();
	FVector PlayerLoc = TargetActor->GetActorLocation();

	// Face the player throughout
	FRotator TargetRot = (PlayerLoc - DragonLoc).Rotation();
	OwnerDragon->SetActorRotation(
		FMath::RInterpTo(OwnerDragon->GetActorRotation(), TargetRot,
			DeltaTime, RotationSpeed));

	// Phase 1: close in until channel range
	if (!bChanneling)
	{
		float Dist = FVector::Dist(DragonLoc, PlayerLoc);

		if (Dist > ChannelDistance)
		{
			Flight->SetAirTarget(PlayerLoc);
			return;
		}

		bChanneling = true;
		Flight->SetAirTarget(DragonLoc);

		if (CycloneMontage)
		{
			OwnerDragon->PlayAnimMontage(CycloneMontage);
		}
	}

	// Phase 2: hover and channel
	Timer += DeltaTime;

	if (!bSpawned && Timer >= SpawnDelay)
	{
		bSpawned = true;

		if (CycloneClass)
		{
			UWorld* World = OwnerDragon->GetWorld();
			if (World)
			{
				FVector SpawnLoc = DragonLoc +
					OwnerDragon->GetActorForwardVector() * SpawnForwardOffset;

				FActorSpawnParameters Params;
				Params.Owner = OwnerDragon;

				ADragonCycloneActor* Cyclone =
					World->SpawnActor<ADragonCycloneActor>(
						CycloneClass, SpawnLoc,
						OwnerDragon->GetActorRotation(), Params);

				if (Cyclone)
				{
					Cyclone->Init(PlayerLoc);
				}
			}
		}
	}

	if (Timer >= SpawnDelay + ChannelTime)
	{
		bFinished = true;
	}
}

bool UDragonAbility_Cyclone::IsFinished() const
{
	return bFinished;
}
