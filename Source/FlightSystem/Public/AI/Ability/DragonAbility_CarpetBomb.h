// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/Ability/DragonAbility.h"
#include "DragonAbility_CarpetBomb.generated.h"

class ADragonBombActor;

/**
 * High-altitude straight pass over the player, dropping a marching line
 * of explosive bombs.
 */
UCLASS()
class FLIGHTSYSTEM_API UDragonAbility_CarpetBomb : public UDragonAbility
{
	GENERATED_BODY()

public:

	virtual void Start(ADragonBaseAI* InOwner, AActor* InTarget) override;
	virtual void Tick(float DeltaTime) override;
	virtual bool IsFinished() const override;
	virtual bool CanBeInterrupted() const override;
	virtual void Abort(EDragonInterruptReason Reason) override;

protected:

	UPROPERTY(EditDefaultsOnly, Category="CarpetBomb")
	float BombAltitude = 2000.f;

	UPROPERTY(EditDefaultsOnly, Category="CarpetBomb")
	float RunLeadDistance = 3500.f;

	UPROPERTY(EditDefaultsOnly, Category="CarpetBomb")
	float RunOvershoot = 2500.f;

	UPROPERTY(EditDefaultsOnly, Category="CarpetBomb")
	float RunSpeed = 5000.f;

	UPROPERTY(EditDefaultsOnly, Category="CarpetBomb")
	int32 BombCount = 7;

	UPROPERTY(EditDefaultsOnly, Category="CarpetBomb")
	UAnimMontage* BombRunMontage;

	UPROPERTY(EditDefaultsOnly, Category="CarpetBomb")
	TSubclassOf<ADragonBombActor> BombClass;

	FVector RunStart;
	FVector RunEnd;
	FVector RunDir;

	bool bRunStarted = false;
	int32 BombsDropped = 0;
	float DropSpacing = 0.f;

	void DropBomb();
};
