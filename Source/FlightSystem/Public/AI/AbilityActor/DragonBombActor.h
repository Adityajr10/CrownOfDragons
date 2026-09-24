// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DragonBombActor.generated.h"

class UNiagaraComponent;
class UNiagaraSystem;
class UCameraShakeBase;

UCLASS()
class FLIGHTSYSTEM_API ADragonBombActor : public AActor
{
	GENERATED_BODY()

public:

	ADragonBombActor();

	virtual void Tick(float DeltaTime) override;

	void Init(const FVector& InVelocity);

protected:

	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere)
	UNiagaraComponent* TrailFX;

	UPROPERTY(EditDefaultsOnly, Category="Bomb")
	UNiagaraSystem* TrailSystem;

	UPROPERTY(EditDefaultsOnly, Category="Bomb")
	UNiagaraSystem* ExplosionSystem;

	UPROPERTY(EditDefaultsOnly, Category="Bomb")
	UNiagaraSystem* LingerFlameSystem;

	UPROPERTY(EditDefaultsOnly, Category="Bomb")
	float Gravity = 3000.f;

	UPROPERTY(EditDefaultsOnly, Category="Bomb")
	float ExplosionRadius = 700.f;

	UPROPERTY(EditDefaultsOnly, Category="Bomb")
	float Damage = 15.f;

	UPROPERTY(EditDefaultsOnly, Category="Bomb")
	float PushForce = 1500.f;

	UPROPERTY(EditDefaultsOnly, Category="Bomb")
	TSubclassOf<UCameraShakeBase> ExplosionCameraShake;

	FVector Velocity;
	bool bExploded = false;

	void Explode(const FVector& ImpactPoint);
};
