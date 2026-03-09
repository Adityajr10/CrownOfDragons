// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/Ability/DragonAbility.h"
#include "DragonAbility_GroundStrafe.generated.h"

class ADragonStrafeFireActor;
/**
 * 
 */
UCLASS()
class FLIGHTSYSTEM_API UDragonAbility_GroundStrafe : public UDragonAbility
{
	GENERATED_BODY()
	virtual void Start(ADragonBaseAI* InOwner, AActor* InTarget) override;
	virtual void Tick(float DeltaTime) override;

protected:

	FVector Start1;
	FVector End;

	FVector MoveDir;

	bool bStarted = false;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ADragonStrafeFireActor> StrafeFireClass;

	ADragonStrafeFireActor* FireActor = nullptr;
};
