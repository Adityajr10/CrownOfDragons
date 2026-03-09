// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Ability/DragonAbility_FlyToAirTarget.h"

void UDragonAbility_FlyToAirTarget::Start(ADragonBaseAI* InOwner, AActor* InTarget)
{
	Super::Start(InOwner, InTarget);

	if (!OwnerDragon)
	{
		bFinished = true;
		return;
	}

	FlightComp = OwnerDragon->FindComponentByClass<UDragonFlightComponent>();

	if (!FlightComp)
	{
		bFinished = true;
		return;
	}
}

void UDragonAbility_FlyToAirTarget::Tick(float DeltaTime)
{
	if (!FlightComp)
	{
		bFinished = true;
		return;
	}

	if (FlightComp->IsAtTarget())
	{
		bFinished = true;
	}
}
