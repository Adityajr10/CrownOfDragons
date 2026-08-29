// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DragonAbility.h"
#include "DragonAbility_LowFlyThreat.generated.h"


class UAnimMontage;
class UDragonFlightComponent;

UCLASS()
class FLIGHTSYSTEM_API UDragonAbility_LowFlyThreat : public UDragonAbility
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

	UPROPERTY(EditDefaultsOnly, Category="Threat")
	UAnimMontage* LowFlyThreatMontage;

	UPROPERTY(EditDefaultsOnly, Category="Threat")
	float ThreatDistance = 900.f;

	UPROPERTY(EditDefaultsOnly, Category="Threat")
	float FlyPastDistance = 1200.f;

	bool bThreatStarted = false;
	bool bFlyPastStarted = false;
	UPROPERTY(EditDefaultsOnly)
	float FlyUpDistance = 300.f;
	
	UPROPERTY(EditDefaultsOnly)
	float LowHeight = 150.f;

	UPROPERTY(EditDefaultsOnly)
	float SideDistance = 1200.f;

	UPROPERTY(EditDefaultsOnly)
	float ClimbHeight = 600.f;

	UPROPERTY(EditDefaultsOnly)
	float SideOffset = 600.f;
	FVector ApproachTarget;
	bool bExitStarted = false;

	FVector ExitTarget;

	FVector FlyPastTarget;
};