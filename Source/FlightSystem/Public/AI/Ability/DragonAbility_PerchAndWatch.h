// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/Ability/DragonAbility.h"
#include "DragonAbility_PerchAndWatch.generated.h"

/**
 * 
 */
UCLASS()
class FLIGHTSYSTEM_API UDragonAbility_PerchAndWatch : public UDragonAbility
{
	GENERATED_BODY()
public:

	virtual void Start(ADragonBaseAI* InOwner, AActor* InTarget) override;
	virtual void Tick(float DeltaTime) override;

private:

	FVector PerchLocation;
	bool bLandingStarted = false;
	bool bLanded = false;
};