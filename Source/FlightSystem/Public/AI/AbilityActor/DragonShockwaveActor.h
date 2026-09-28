// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DragonShockwaveActor.generated.h"

class UNiagaraComponent;
class UNiagaraSystem;
class UCameraShakeBase;

UCLASS()
class FLIGHTSYSTEM_API ADragonShockwaveActor : public AActor
{
	GENERATED_BODY()

public:

	ADragonShockwaveActor();

	virtual void Tick(float DeltaTime) override;

	// Optional: wave travels toward a point while expanding (ConcussiveRoar)
	void InitTravel(const FVector& InTarget);

protected:

	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere)
	UNiagaraComponent* ShockwaveFX;

	UPROPERTY(EditDefaultsOnly, Category="Shockwave")
	UNiagaraSystem* ShockwaveSystem;

	UPROPERTY(EditDefaultsOnly, Category="Shockwave")
	float MaxRadius = 3000.f;

	UPROPERTY(EditDefaultsOnly, Category="Shockwave")
	float ExpandSpeed = 6000.f;

	UPROPERTY(EditDefaultsOnly, Category="Shockwave")
	float PushForce = 3000.f;

	UPROPERTY(EditDefaultsOnly, Category="Shockwave")
	float LaunchUpForce = 400.f;

	UPROPERTY(EditDefaultsOnly, Category="Shockwave")
	float Damage = 10.f;

	UPROPERTY(EditDefaultsOnly, Category="Shockwave")
	TSubclassOf<UCameraShakeBase> ImpactCameraShake;

	UPROPERTY(EditDefaultsOnly, Category="Shockwave")
	float TravelSpeed = 4000.f;

	FVector TravelTarget;
	bool bTravel = false;

	float CurrentRadius = 0.f;
	bool bPlayerHit = false;
	bool bExpansionDone = false;
};
