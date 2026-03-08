// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Ability/DragonAbility.h"
#include "FlightSystem/DragonBaseAI.h"
void UDragonAbility::Start(ADragonBaseAI* InOwner, AActor* InTarget)
{
	OwnerDragon = InOwner;
	TargetActor = InTarget;
	bFinished = false;
}

void UDragonAbility::Tick(float DeltaTime)
{
	// Base ability does nothing
	// Derived abilities override this
}

bool UDragonAbility::IsFinished() const
{
	return bFinished;
}
