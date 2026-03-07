// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DragonFlightInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UDragonFlightInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class FLIGHTSYSTEM_API IDragonFlightInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FVector GetRandomAirPoint(float Radius, float MinHeight, float MaxHeight);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetCurrentAirTarget(FVector Target);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FVector GetCurrentAirTarget();
};