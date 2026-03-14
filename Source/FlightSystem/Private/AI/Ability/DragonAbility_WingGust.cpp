// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Ability/DragonAbility_WingGust.h"
#include "AI/Ability/DragonAbility_WingGust.h"
#include "AI/AbilityActor/DragonWingGustActor.h"

void UDragonAbility_WingGust::SetGustMode(EWingGustMode InMode)
{
	GustMode = InMode;
}

void UDragonAbility_WingGust::Start(ADragonBaseAI* InOwner, AActor* InTarget)
{
	Super::Start(InOwner, InTarget);

	bExecuted = false;

	if (OwnerDragon && WingGustMontage)
	{
		OwnerDragon->PlayAnimMontage(WingGustMontage);
	}
}

void UDragonAbility_WingGust::Tick(float DeltaTime)
{
	if (!OwnerDragon) return;

	if (!bExecuted)
	{
		bExecuted = true;

		if (!WingGustActorClass) 
		{
			bFinished = true;
			return;
		}

		FVector SpawnLoc = OwnerDragon->GetActorLocation();

		// If gust is used in air spawn slightly below dragon
		if (GustMode == EWingGustMode::Air)
		{
			SpawnLoc.Z -= 200.f;
		}

		UWorld* World = OwnerDragon->GetWorld();
		if (!World)
		{
			bFinished = true;
			return;
		}

		World->SpawnActor<ADragonWingGustActor>(
			WingGustActorClass,
			SpawnLoc,
			OwnerDragon->GetActorRotation()
		);

		bFinished = true;
	}
}

bool UDragonAbility_WingGust::IsFinished() const
{
	return bFinished;
}