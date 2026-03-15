// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DragonAbility.h"
#include "DragonAbility_ThermalGlide.generated.h"

/**
 * 
 */
UCLASS()
class FLIGHTSYSTEM_API UDragonAbility_ThermalGlide : public UDragonAbility
{
	GENERATED_BODY()
public:

	virtual void Start(ADragonBaseAI* InOwner, AActor* InTarget) override;
	virtual void Tick(float DeltaTime) override;

	/* Returns true when ability is finished */
	virtual bool IsFinished() const override;

protected:

	UPROPERTY(EditDefaultsOnly, Category="Glide")
	float GlideDistance = 5000.f;

	UPROPERTY(EditDefaultsOnly, Category="Glide")
	float GlideHeight = 3200.f;

	UPROPERTY(EditDefaultsOnly, Category="Glide")
	float GlideDuration = 6.f;

	float AbilityTimer = 0.f;

	FVector GlideTarget;
};