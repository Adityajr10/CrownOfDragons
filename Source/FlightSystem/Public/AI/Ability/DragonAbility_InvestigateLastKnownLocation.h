// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DragonAbility.h"
#include "DragonAbility_InvestigateLastKnownLocation.generated.h"

/**
 * 
 */
UCLASS()
class FLIGHTSYSTEM_API UDragonAbility_InvestigateLastKnownLocation : public UDragonAbility
{
	GENERATED_BODY()
public:

	void SetInvestigateLocation(const FVector& InLocation);

	virtual void Start(ADragonBaseAI* InOwner, AActor* InTarget) override;
	virtual void Tick(float DeltaTime) override;
	virtual bool IsFinished() const override;

protected:

	UPROPERTY(EditDefaultsOnly, Category="Investigate")
	float InspectDuration = 3.0f;

	UPROPERTY(EditDefaultsOnly, Category="Investigate")
	float ReachDistance = 300.f;

	FVector InvestigateLocation;

	bool bReachedLocation = false;

	float InspectTimer = 0.f;
};