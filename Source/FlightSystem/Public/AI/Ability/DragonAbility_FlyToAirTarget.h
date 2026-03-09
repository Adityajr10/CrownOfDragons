// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/Ability/DragonAbility.h"
#include "DragonAbility_FlyToAirTarget.generated.h"

/**
 * 
 */
UCLASS()
class FLIGHTSYSTEM_API UDragonAbility_FlyToAirTarget : public UDragonAbility
{
	GENERATED_BODY()
	virtual void Start(ADragonBaseAI* InOwner, AActor* InTarget) override;
	virtual void Tick(float DeltaTime) override;

private:

	UDragonFlightComponent* FlightComp = nullptr;
};