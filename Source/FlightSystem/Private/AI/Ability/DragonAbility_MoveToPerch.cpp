// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Ability/DragonAbility_MoveToPerch.h"
void UDragonAbility_MoveToPerch::SetPerchLocation(const FVector& InLocation)
{
	//PerchLocation = InLocation;
	PerchLocation =FVector(36988.972731, 22267.368020, 7439.448210);
}

void UDragonAbility_MoveToPerch::Start(
	ADragonBaseAI* InOwner,
	AActor* InTarget)
{
	Super::Start(InOwner, InTarget);

	bLandingStarted = false;
	bRestStarted = false;
}

void UDragonAbility_MoveToPerch::Tick(float DeltaTime)
{
	if (!OwnerDragon) return;

	UDragonFlightComponent* Flight =
		OwnerDragon->FindComponentByClass<UDragonFlightComponent>();

	if (!Flight) return;

	/* Fly toward perch */
	if (!bLandingStarted)
	{
		Flight->SetAirTarget(PerchLocation);

		float Dist =
			FVector::Dist(
				OwnerDragon->GetActorLocation(),
				PerchLocation);

		if (Dist < ReachDistance)
		{
			bLandingStarted = true;

			if (LandingMontage)
			{
				OwnerDragon->PlayAnimMontage(LandingMontage);
			}

			OwnerDragon->IsFlying = false;
		}

		return;
	}

	/* Wait for landing montage */
	if (!bRestStarted &&
		!OwnerDragon->GetMesh()
		->GetAnimInstance()
		->Montage_IsPlaying(LandingMontage))
	{
		bRestStarted = true;

		if (RestMontage)
		{
			OwnerDragon->PlayAnimMontage(RestMontage);
		}

		return;
	}

	/* Finish after rest animation */
	if (bRestStarted &&
		!OwnerDragon->GetMesh()
		->GetAnimInstance()
		->Montage_IsPlaying(RestMontage))
	{
		bFinished = true;
	}
}

bool UDragonAbility_MoveToPerch::IsFinished() const
{
	return bFinished;
}