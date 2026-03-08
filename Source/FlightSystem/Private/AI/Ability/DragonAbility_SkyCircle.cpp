// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Ability/DragonAbility_SkyCircle.h"
#include "FlightSystem/DragonFlightComponent.h"

void UDragonAbility_SkyCircle::Start(ADragonBaseAI* InOwner, AActor* InTarget)
{
	Super::Start(InOwner, InTarget);

	CircleAngle = 0.f;
}

void UDragonAbility_SkyCircle::Tick(float DeltaTime)
{
	if (!OwnerDragon) return;

	UDragonFlightComponent* Flight = OwnerDragon->FindComponentByClass<UDragonFlightComponent>();
	if (!Flight) return;

	CircleAngle += DeltaTime * 0.4f;

	FVector Center = OwnerDragon->StartLocation;
	float Height = FMath::FRandRange(
		OwnerDragon->CircleHeightMin,
		OwnerDragon->CircleHeightMax
	);

	FVector Target;
	Target.X = Center.X + FMath::Cos(CircleAngle) * OwnerDragon->CircleRadius;
	Target.Y = Center.Y + FMath::Sin(CircleAngle) * OwnerDragon->CircleRadius;
	Target.Z = Center.Z + Height;

	Flight->SetAirTarget(Target);
}
