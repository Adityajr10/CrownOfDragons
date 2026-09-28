// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DragonAbility.h"
#include "DragonAbility_FireRain.generated.h"

class ADragonFireActor;
class ADragonFireRainActor;
/**
 * 
 */
UCLASS()
class FLIGHTSYSTEM_API UDragonAbility_FireRain : public UDragonAbility
{
	GENERATED_BODY()
public:

	void SetTargetLocation(const FVector& InLocation);

	virtual void Start(ADragonBaseAI* InOwner, AActor* InTarget) override;
	virtual void Tick(float DeltaTime) override;

	/* Returns true when ability is finished */
	virtual bool IsFinished() const override;

protected:

	UPROPERTY(EditDefaultsOnly, Category="Fire")
	TSubclassOf<ADragonFireActor> FireActorClass;

	UPROPERTY(EditDefaultsOnly, Category="Fire")
	UAnimMontage* FireRainMontage;

	UPROPERTY(EditDefaultsOnly, Category="Fire")
	float ReachDistance = 300.f;

	UPROPERTY(EditDefaultsOnly, Category="Fire")
	float FireDelay = 0.3f;
	
	UPROPERTY(EditDefaultsOnly, Category="Fire")
	float FireRainHeight = 4000.f;

	FVector TargetLocation;

	bool bReachedLocation = false;
	bool bMontageStarted = false;
	bool bFireSpawned = false;

	float DelayTimer = 0.f;
	
	virtual bool CanBeInterrupted() const override;
	virtual void Abort(EDragonInterruptReason Reason) override;
	virtual void OnOwnerDamaged(float DamageAmount, float DamagePercent, AActor* Instigator) override;
};