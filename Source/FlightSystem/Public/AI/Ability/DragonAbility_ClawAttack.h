// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DragonAbility.h"
#include "DragonAbility_ClawAttack.generated.h"


class UDragonFlightComponent;
class UAnimMontage;

UCLASS()
class FLIGHTSYSTEM_API UDragonAbility_ClawAttack : public UDragonAbility
{
	GENERATED_BODY()
public:

	virtual void Start(ADragonBaseAI* InOwner, AActor* InTarget) override;
	virtual void Tick(float DeltaTime) override;

	/* Returns true when ability is finished */
	virtual bool IsFinished() const override;
	
	virtual bool CanBeInterrupted() const override;
	virtual void Abort(EDragonInterruptReason Reason) override;

protected:

	/* Claw attack animation */
	UPROPERTY(EditDefaultsOnly, Category="Attack")
	UAnimMontage* ClawAttackMontage;

	UPROPERTY(EditDefaultsOnly, Category="Attack")
	float AttackDistance = 350.f;

	UPROPERTY(EditDefaultsOnly)
	float LungeDistance = 400.f;

	UPROPERTY(EditDefaultsOnly)
	float GlideDistance = 900.f;

	bool bGlideStarted = false;
	bool bAttackStarted = false;
	bool bLungeStarted = false;

	UPROPERTY(EditDefaultsOnly)
	float GlideUpStrength = 0.4f;

	FVector GlideTarget;
	float GlideTimer = 0.f;

	UPROPERTY(EditDefaultsOnly, Category="ClawAttack")
	float MinGlideDuration = 1.5f;
};