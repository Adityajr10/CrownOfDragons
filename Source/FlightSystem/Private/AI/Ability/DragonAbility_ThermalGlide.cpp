// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Ability/DragonAbility_ThermalGlide.h"
void UDragonAbility_ThermalGlide::Start(
	ADragonBaseAI* InOwner,
	AActor* InTarget)
{
	Super::Start(InOwner, InTarget);

	AbilityTimer = 0.f;

	if (!OwnerDragon) return;

	FVector Forward = OwnerDragon->GetActorForwardVector();

	FVector StartLoc = OwnerDragon->GetActorLocation();

	GlideTarget = StartLoc + Forward * GlideDistance;
	GlideTarget.Z = GlideHeight;
}

void UDragonAbility_ThermalGlide::Tick(float DeltaTime)
{
	if (!OwnerDragon) return;

	UDragonFlightComponent* Flight =
		OwnerDragon->FindComponentByClass<UDragonFlightComponent>();

	if (!Flight) return;

	AbilityTimer += DeltaTime;

	/* Glide forward */
	Flight->SetAirTarget(GlideTarget);

	/* Maintain altitude */
	FVector Current = OwnerDragon->GetActorLocation();
	Current.Z = GlideHeight;
	OwnerDragon->SetActorLocation(Current);

	if (AbilityTimer > GlideDuration)
	{
		bFinished = true;
	}
}

bool UDragonAbility_ThermalGlide::IsFinished() const
{
	return bFinished;
}