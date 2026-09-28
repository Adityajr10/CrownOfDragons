// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DragonAbility.h"
#include "DragonAbility_AirStrafe.generated.h"

/**
 * 
 */
UCLASS()
class FLIGHTSYSTEM_API UDragonAbility_AirStrafe : public UDragonAbility
{
	GENERATED_BODY()
	public:
	void SetStrafeLocation(const FVector& InLocation);

	virtual void Start(ADragonBaseAI* InOwner, AActor* InTarget) override;
	virtual void Tick(float DeltaTime) override;
	virtual bool IsFinished() const override;
	
	virtual bool CanBeInterrupted() const override;
	virtual void Abort(EDragonInterruptReason Reason) override;

private:

	FVector StrafeLocation;
	// Add these to the protected section of your header:

	UPROPERTY(EditDefaultsOnly, Category="AirStrafe")
	float StrafeRadiusMin = 3000.f;

	UPROPERTY(EditDefaultsOnly, Category="AirStrafe")
	float StrafeRadiusMax = 4000.f;

	bool bHasCustomLocation = false;
};
