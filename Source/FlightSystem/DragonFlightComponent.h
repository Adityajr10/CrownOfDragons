// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DragonFlightComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FLIGHTSYSTEM_API UDragonFlightComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDragonFlightComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Generates true 3D air waypoint
	UFUNCTION(BlueprintCallable)
	FVector GetRandomAirPoint(float Radius, float MinHeight, float MaxHeight);

	// Sets new air destination
	UFUNCTION(BlueprintCallable)
	void SetAirTarget(FVector Target);

	// Used by BTTask to know when target reached
	UFUNCTION(BlueprintCallable)
	bool IsAtTarget() const { return !bHasTarget; }

private:
	FVector CurrentTarget;
	bool bHasTarget = false;
};