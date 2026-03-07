// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraSystem.h"
#include "GameFramework/Actor.h"
#include "DragonFlameBeam.generated.h"

class UProjectileMovementComponent;

UCLASS()
class FLIGHTSYSTEM_API ADragonFlameBeam : public AActor
{
	GENERATED_BODY()
	
public:
	ADragonFlameBeam();

	void Init(FVector Start, FVector Dir);

	// 🔥 THIS is the setter
	FORCEINLINE void SetFireFX(UNiagaraSystem* InFX) { FireFX = InFX; }

protected:
	UPROPERTY()
	UNiagaraSystem* FireFX;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere)
	UNiagaraComponent* FX;

	UPROPERTY(VisibleAnywhere)
	UProjectileMovementComponent* Projectile;
};