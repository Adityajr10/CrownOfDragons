// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DragonAbility.h"
#include "AI/Task/BTTask_WingGust.h"
#include "DragonAbility_WingGust.generated.h"

class ADragonWingGustActor;
/**
 * 
 */
UCLASS()
class FLIGHTSYSTEM_API UDragonAbility_WingGust : public UDragonAbility
{
	GENERATED_BODY()
public:

	virtual void Start(ADragonBaseAI* InOwner, AActor* InTarget) override;
	virtual void Tick(float DeltaTime) override;

	virtual bool IsFinished() const override;

protected:

	UPROPERTY(EditDefaultsOnly, Category="WingGust")
	float AttackDistance = 600.f;

	UPROPERTY(EditDefaultsOnly, Category="WingGust")
	float SpawnDelay = 0.4f;

	UPROPERTY(EditDefaultsOnly, Category="WingGust")
	float GustDistance = 1500.f;

	UPROPERTY(EditDefaultsOnly, Category="WingGust")
	UAnimMontage* WingGustMontage;

	UPROPERTY(EditDefaultsOnly, Category="WingGust")
	TSubclassOf<ADragonWingGustActor> WingGustActorClass;

	bool bMontageStarted = false;
	bool bSpawned = false;

	float Timer = 0.f;
};