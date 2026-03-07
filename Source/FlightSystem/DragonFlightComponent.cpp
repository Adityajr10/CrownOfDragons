// Fill out your copyright notice in the Description page of Project Settings.


#include "DragonFlightComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

UDragonFlightComponent::UDragonFlightComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UDragonFlightComponent::BeginPlay()
{
	Super::BeginPlay();

	ACharacter* Char = Cast<ACharacter>(GetOwner());
	if (Char)
	{
		UCharacterMovementComponent* Move = Char->GetCharacterMovement();
		Move->SetMovementMode(MOVE_Flying);
		Move->MaxFlySpeed = 1500.f;
		Move->BrakingDecelerationFlying = 200.f;
	}
}

FVector UDragonFlightComponent::GetRandomAirPoint(float Radius, float MinHeight, float MaxHeight)
{
	FVector Origin = GetOwner()->GetActorLocation();
	FVector Dir = UKismetMathLibrary::RandomUnitVector();
	float Dist = FMath::FRandRange(300.f, Radius);

	FVector P = Origin + Dir * Dist;
	P.Z += FMath::FRandRange(MinHeight, MaxHeight);
	return P;
}

void UDragonFlightComponent::SetAirTarget(FVector Target)
{
	CurrentTarget = Target;
	bHasTarget = true;
}

void UDragonFlightComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bHasTarget) return;

	ACharacter* Char = Cast<ACharacter>(GetOwner());
	if (!Char) return;

	FVector Dir = (CurrentTarget - Char->GetActorLocation()).GetSafeNormal();
	FVector DesiredVel = Dir * Char->GetCharacterMovement()->MaxFlySpeed;

	Char->GetCharacterMovement()->Velocity =
		FMath::VInterpTo(Char->GetCharacterMovement()->Velocity, DesiredVel, DeltaTime, 2.f);

	FRotator Look = UKismetMathLibrary::FindLookAtRotation(Char->GetActorLocation(), CurrentTarget);
	Char->SetActorRotation(FMath::RInterpTo(Char->GetActorRotation(), Look, DeltaTime, 2.f));

	if (FVector::Dist(Char->GetActorLocation(), CurrentTarget) < 300.f)
	{
		bHasTarget = false;
	}
}