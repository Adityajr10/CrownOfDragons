// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DragonFireRainActor.generated.h"

class UNiagaraComponent;
class UNiagaraSystem;

UCLASS()
class FLIGHTSYSTEM_API ADragonFireRainActor : public AActor
{
	GENERATED_BODY()


public:

	ADragonFireRainActor();

	virtual void Tick(float DeltaTime) override;

protected:

	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere)
	UNiagaraComponent* NiagaraComp;

	UPROPERTY(EditDefaultsOnly)
	UNiagaraSystem* FireFX;

	UPROPERTY(EditDefaultsOnly)
	float FallSpeed = 4000.f;

	UPROPERTY(EditDefaultsOnly)
	float DamageRadius = 350.f;

	UPROPERTY(EditDefaultsOnly)
	float Damage = 40.f;

	void Explode();
};