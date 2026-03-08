// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DragonFireActor.generated.h"

class UNiagaraSystem;
class UNiagaraComponent;

UCLASS()
class FLIGHTSYSTEM_API ADragonFireActor : public AActor
{
	GENERATED_BODY()
	
public:

	ADragonFireActor();

	virtual void Tick(float DeltaTime) override;

	void Init(FVector Direction);

protected:

	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere)
	UNiagaraComponent* NiagaraComp;

	UPROPERTY(EditDefaultsOnly, Category="Fire")
	UNiagaraSystem* FireFX;

	FVector MoveDir;

	UPROPERTY(EditDefaultsOnly, Category="Fire")
	float Speed = 3500.f;
};