// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/Ability/DragonAbility.h"
#include "DragonAbility_Cyclone.generated.h"

class ADragonCycloneActor;

/**
 * Hover in place and beat the wings into a tornado that drifts toward
 * the player, pushing them around.
 */
UCLASS()
class FLIGHTSYSTEM_API UDragonAbility_Cyclone : public UDragonAbility
{
	GENERATED_BODY()

public:

	virtual void Start(ADragonBaseAI* InOwner, AActor* InTarget) override;
	virtual void Tick(float DeltaTime) override;
	virtual bool IsFinished() const override;

protected:

	UPROPERTY(EditDefaultsOnly, Category="Cyclone")
	float ChannelDistance = 2500.f;

	UPROPERTY(EditDefaultsOnly, Category="Cyclone")
	float ChannelTime = 2.0f;

	UPROPERTY(EditDefaultsOnly, Category="Cyclone")
	float SpawnDelay = 0.6f;

	UPROPERTY(EditDefaultsOnly, Category="Cyclone")
	float SpawnForwardOffset = 800.f;

	UPROPERTY(EditDefaultsOnly, Category="Cyclone")
	float RotationSpeed = 4.f;

	UPROPERTY(EditDefaultsOnly, Category="Cyclone")
	UAnimMontage* CycloneMontage;

	UPROPERTY(EditDefaultsOnly, Category="Cyclone")
	TSubclassOf<ADragonCycloneActor> CycloneClass;

	bool bChanneling = false;
	bool bSpawned = false;
	float Timer = 0.f;
};
