// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DragonStrafeFireActor.generated.h"

class UNiagaraSystem;
class UNiagaraComponent;

UCLASS()
class FLIGHTSYSTEM_API ADragonStrafeFireActor : public AActor
{
	GENERATED_BODY()
public:

	ADragonStrafeFireActor();

	virtual void Tick(float DeltaTime) override;

	void StopFire();

protected:

	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere)
	UNiagaraComponent* NiagaraComp;

	UPROPERTY(EditDefaultsOnly)
	UNiagaraSystem* FireFX;

	UPROPERTY(EditDefaultsOnly)
	float DamageRadius = 500.f;

	UPROPERTY(EditDefaultsOnly)
	float DamagePerSecond = 25.f;

	AActor* Player;
};