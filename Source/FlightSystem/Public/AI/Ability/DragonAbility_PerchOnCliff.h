// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DragonAbility.h"
#include "DragonAbility_PerchOnCliff.generated.h"

/**
 * 
 */
UCLASS()
class FLIGHTSYSTEM_API UDragonAbility_PerchOnCliff : public UDragonAbility
{
	GENERATED_BODY()
public:

	void SetPerchLocation(const FVector& InLocation);

	virtual void Start(ADragonBaseAI* InOwner, AActor* InTarget) override;
	virtual void Tick(float DeltaTime) override;

	/* Returns true when ability is finished */
	virtual bool IsFinished() const override;

protected:

	UPROPERTY(EditDefaultsOnly, Category="Perch")
	float LandingDistance = 250.f;

	UPROPERTY(EditDefaultsOnly, Category="Perch")
	UAnimMontage* LandingMontage;

	FVector PerchLocation;

	bool bLandingStarted = false;
	
	UPROPERTY(EditDefaultsOnly, Category="Perch")
	float PostLandWaitDuration = 3.f;
	bool  bMontageFinished  = false;
	bool  bFinished         = false;
	float PostLandTimer     = 0.f;
};