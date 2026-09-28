// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DragonAbility.h"
#include "DragonAbility_MoveToPerch.generated.h"

/**
 * 
 */
UCLASS()
class FLIGHTSYSTEM_API UDragonAbility_MoveToPerch : public UDragonAbility
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
	float ReachDistance = 250.f;

	UPROPERTY(EditDefaultsOnly, Category="Perch")
	UAnimMontage* LandingMontage;

	UPROPERTY(EditDefaultsOnly, Category="Perch")
	UAnimMontage* RestMontage;

	FVector PerchLocation;

	bool bLandingStarted = false;
	bool bRestStarted = false;
};