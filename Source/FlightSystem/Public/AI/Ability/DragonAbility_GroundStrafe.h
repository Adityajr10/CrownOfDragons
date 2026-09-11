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
	virtual bool IsFinished() const override;

protected:

	FVector Start1;
	FVector End;

	FVector MoveDir;

	bool bStarted = false;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ADragonStrafeFireActor> StrafeFireClass;

	ADragonStrafeFireActor* FireActor = nullptr;
	
	// Strafe path points
	FVector EntryPoint;
	FVector ExitPoint;
	FVector StrafeDir;
 
	bool bReachedEntry = false;
	bool bStrafeStarted = false;
 
	// How far from the player the dragon starts its strafe run
	UPROPERTY(EditDefaultsOnly, Category="GroundStrafe")
	float StrafeApproachDistance = 3000.f;
 
	// How far past the player the dragon flies after passing
	UPROPERTY(EditDefaultsOnly, Category="GroundStrafe")
	float StrafeRunDistance = 2000.f;
	
protected:
	FVector StrafeLocation;

	UPROPERTY(EditDefaultsOnly, Category="GroundStrafe")
	float StrafeRadiusMin = 2000.f;

	UPROPERTY(EditDefaultsOnly, Category="GroundStrafe")
	float StrafeRadiusMax = 3000.f;

	// Low altitude range — stays close to player height
	UPROPERTY(EditDefaultsOnly, Category="GroundStrafe")
	float LowHeightMin = 200.f;

	UPROPERTY(EditDefaultsOnly, Category="GroundStrafe")
	float LowHeightMax = 500.f;
	
	float StrafeTimer = 0.f;

	UPROPERTY(EditDefaultsOnly, Category="GroundStrafe")
	float MinStrafeDuration = 3.0f;
 
};
