// Fill out your copyright notice in the Description page of Project Settings.


#include "DragonBaseAI.h"
#include "DragonFlightComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

ADragonBaseAI::ADragonBaseAI()
{
	PrimaryActorTick.bCanEverTick = true;

	// Add flight brain
	FlightComponent = CreateDefaultSubobject<UDragonFlightComponent>(TEXT("FlightComponent"));
	FireBreath = CreateDefaultSubobject<UDragonFireBreathComponent>(TEXT("FireBreath"));


	// Base flight setup
//	GetCharacterMovement()->SetMovementMode(MOVE_Flying);
//	GetCharacterMovement()->GravityScale = 0.f;
	//GetCharacterMovement()->MaxFlySpeed = 1500.f;
	//GetCharacterMovement()->BrakingDecelerationFlying = 200.f;

	// bUseControllerRotationYaw = false;
}

void ADragonBaseAI::BeginPlay()
{
	Super::BeginPlay();
	StartLocation = GetActorLocation(); 
}

void ADragonBaseAI::TakeoffFromPerch()
{
	IsFlying = true;

	if (TakeoffMontage)
		PlayAnimMontage(TakeoffMontage);

	FVector UpBoost = GetActorUpVector() * 3500.f;
	LaunchCharacter(UpBoost, true, true);
}

//PERCH DIVE-BOMB SYSTEM
void ADragonBaseAI::StartDiveBomb(AActor* Target)
{
	if (!Target) return;

	IsFlying = true;

	if (DiveStartMontage)
		PlayAnimMontage(DiveStartMontage);

	FVector AboveTarget = Target->GetActorLocation();
	AboveTarget.Z += DiveHeight;

	SetActorLocation(AboveTarget);

	GetCharacterMovement()->Velocity = FVector::ZeroVector;

	FVector DiveDir = (Target->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	LaunchCharacter(DiveDir * DiveSpeed, true, true);
}

void ADragonBaseAI::OnDiveImpact()
{
	if (DiveEndMontage)
		PlayAnimMontage(DiveEndMontage);

	// TODO: apply damage, knockback, explosion FX here
}

