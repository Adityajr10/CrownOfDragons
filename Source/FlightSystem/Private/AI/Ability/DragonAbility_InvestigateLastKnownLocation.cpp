// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Ability/DragonAbility_InvestigateLastKnownLocation.h"

void UDragonAbility_InvestigateLastKnownLocation::SetInvestigateLocation(
	const FVector& InLocation)
{
	//InvestigateLocation = InLocation;
	InvestigateLocation = FVector(36988.972731, 22267.368020, 7439.448210);
}

void UDragonAbility_InvestigateLastKnownLocation::Start(
	ADragonBaseAI* InOwner,
	AActor* InTarget)
{
	Super::Start(InOwner, InTarget);
	AbilityType = EDragonAbilityType::InvestigateLastKnownLocation;
	bReachedLocation = false;
	InspectTimer = 0.f;
}

void UDragonAbility_InvestigateLastKnownLocation::Tick(float DeltaTime)
{
	if (!OwnerDragon) return;

	UDragonFlightComponent* Flight =
		OwnerDragon->FindComponentByClass<UDragonFlightComponent>();

	if (!Flight) return;

	if (!bReachedLocation)
	{
		Flight->SetAirTarget(InvestigateLocation);

		float Dist =
			FVector::Dist(
				OwnerDragon->GetActorLocation(),
				InvestigateLocation);

		if (Dist < ReachDistance)
		{
			bReachedLocation = true;
		}

		return;
	}

	InspectTimer += DeltaTime;

	if (InspectTimer > InspectDuration)
	{
		bFinished = true;
	}
}

bool UDragonAbility_InvestigateLastKnownLocation::IsFinished() const
{
	return bFinished;
}