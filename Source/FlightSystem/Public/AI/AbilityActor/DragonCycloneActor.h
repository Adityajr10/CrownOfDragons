// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DragonCycloneActor.generated.h"

class UNiagaraComponent;
class UNiagaraSystem;

UCLASS()
class FLIGHTSYSTEM_API ADragonCycloneActor : public AActor
{
	GENERATED_BODY()

public:

	ADragonCycloneActor();

	virtual void Tick(float DeltaTime) override;

	void Init(const FVector& InTarget);

protected:

	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere)
	UNiagaraComponent* CycloneFX;

	UPROPERTY(EditDefaultsOnly, Category="Cyclone")
	UNiagaraSystem* CycloneSystem;

	UPROPERTY(EditDefaultsOnly, Category="Cyclone")
	float PushRadius = 900.f;

	UPROPERTY(EditDefaultsOnly, Category="Cyclone")
	float PushForce = 1800.f;

	UPROPERTY(EditDefaultsOnly, Category="Cyclone")
	float PushInterval = 0.4f;

	UPROPERTY(EditDefaultsOnly, Category="Cyclone")
	float HomingSpeed = 600.f;

	UPROPERTY(EditDefaultsOnly, Category="Cyclone")
	float LifeDuration = 8.f;

	float LifeTimer = 0.f;
	float PushTimer = 0.f;
	FVector TargetLocation;
	bool bHasTarget = false;

	void ApplyPush();
};
