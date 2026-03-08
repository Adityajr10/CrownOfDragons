// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DragonFireBreathComponent.h"
#include "DragonFlightComponent.h"
#include "GameFramework/Character.h"
#include "DragonBaseAI.generated.h"

class UDragonAbilityComponent;

UCLASS()
class FLIGHTSYSTEM_API ADragonBaseAI : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
public:
	ADragonBaseAI();


protected:
	virtual void BeginPlay() override;
	

public:
	void TakeoffFromPerch();
	void StartDiveBomb(AActor* Target);
	void OnDiveImpact();
	// Plug-in flight brain
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UDragonFlightComponent* FlightComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UDragonFireBreathComponent* FireBreath;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dragon|Ability")
	UDragonAbilityComponent* AbilityComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsFlying;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Canplay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector StartLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Flight|Patrol")
	float CircleRadius = 12000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Flight|Patrol")
	float CircleHeightMin = 3000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Flight|Patrol")
	float CircleHeightMax = 7000.f;

	float CircleAngle = 0.f;


	//Perch & Watch (Intimidation Mode)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Perch")
	float PerchHeightOffset = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Perch")
	float PerchDuration = 5.f;

	bool bIsPerched = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Perch|Anim")
	UAnimMontage* WingFoldMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Perch|Anim")
	UAnimMontage* PerchRoarMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Perch|Anim")
	UAnimMontage* TakeoffMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Perch")
	float PerchWatchTime = 6.f;

	//PERCH DIVE-BOMB SYSTEM

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="DiveBomb")
	float DiveHeight = 9000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="DiveBomb")
	float DiveSpeed = 9000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="DiveBomb")
	UAnimMontage* DiveStartMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="DiveBomb")
	UAnimMontage* DiveEndMontage;

	AActor* DiveTarget = nullptr;
	bool bIsClimbing = false;
	bool bIsDiving = false;

	//GroundStrafe
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Strafe")
	float StrafeHeight = 400.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Strafe")
	float StrafeDistance = 12000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Strafe")
	float StrafeSpeed = 3500.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Strafe")
	UAnimMontage* StrafeMontage;
};