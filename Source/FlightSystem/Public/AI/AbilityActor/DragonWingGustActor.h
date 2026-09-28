// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DragonWingGustActor.generated.h"

class UNiagaraComponent;
class UNiagaraSystem;
UCLASS()
class FLIGHTSYSTEM_API ADragonWingGustActor : public AActor
{
	GENERATED_BODY()

public:

	ADragonWingGustActor();

	virtual void Tick(float DeltaTime) override;

protected:

	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere)
	UNiagaraComponent* GustFX;

	UPROPERTY(EditDefaultsOnly, Category="Gust")
	UNiagaraSystem* GustSystem;

	UPROPERTY(EditDefaultsOnly, Category="Gust")
	float GustRadius = 1200.f;

	UPROPERTY(EditDefaultsOnly, Category="Gust")
	float PushForce = 2500.f;

	UPROPERTY(EditDefaultsOnly, Category="Gust")
	float GustDuration = 0.5f;

	float LifeTimer = 0.f;
	float LifeDuration = 4.f;
	void ApplyPush();
	
	FVector TargetLocation;
public:
	void Init(const FVector& InTarget);
	UPROPERTY(EditDefaultsOnly, Category="WingGust")
	float Speed = 2000.f;
};