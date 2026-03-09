// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/Ability/DragonAbility.h"
#include "DragonAbility_PlayerOrbitStrafe.generated.h"

class UDragonFlightComponent;
class UDragonFireBreathComponent;
UCLASS()
class FLIGHTSYSTEM_API UDragonAbility_PlayerOrbitStrafe : public UDragonAbility
{
	GENERATED_BODY()
	
public:

	virtual void Start(ADragonBaseAI* InOwner, AActor* InTarget) override;
	virtual void Tick(float DeltaTime) override;
	virtual void End();

protected:

	UPROPERTY()
	UDragonFlightComponent* FlightComp = nullptr;

	UPROPERTY()
	UDragonFireBreathComponent* FireComp = nullptr;

	float OrbitRadius = 3500.f;
	float OrbitHeight = 1500.f;
};