#include "AI/Ability/DragonAbility_AirStrafe.h"

void UDragonAbility_AirStrafe::SetStrafeLocation(const FVector& InLocation)
{
	StrafeLocation = InLocation;
	bHasCustomLocation = true;
}

void UDragonAbility_AirStrafe::Start(ADragonBaseAI* InOwner, AActor* InTarget)
{
	Super::Start(InOwner, InTarget);
	AbilityType = EDragonAbilityType::AirStrafe;
	bHasCustomLocation = false;

	if (!OwnerDragon) return;

	// Auto-pick strafe location if none was set externally
	FVector DragonLoc = OwnerDragon->GetActorLocation();

	if (TargetActor)
	{
		// Strafe AWAY from target — pick a point behind/beside the dragon
		FVector AwayFromTarget =
			(DragonLoc - TargetActor->GetActorLocation()).GetSafeNormal();

		// Random sideways offset (-60 to +60 degrees) so it doesn't
		// always fly straight back — looks more natural
		float RandomAngle = FMath::FRandRange(-60.f, 60.f);
		FVector Rotated = AwayFromTarget.RotateAngleAxis(RandomAngle, FVector::UpVector);

		float RandomDist = FMath::FRandRange(StrafeRadiusMin, StrafeRadiusMax);

		StrafeLocation = DragonLoc + Rotated * RandomDist;
	}
	else
	{
		// No target — pick completely random direction
		float RandomAngle = FMath::FRandRange(0.f, 360.f);
		FVector RandomDir = FVector(
			FMath::Cos(FMath::DegreesToRadians(RandomAngle)),
			FMath::Sin(FMath::DegreesToRadians(RandomAngle)),
			0.f
		);

		float RandomDist = FMath::FRandRange(StrafeRadiusMin, StrafeRadiusMax);

		StrafeLocation = DragonLoc + RandomDir * RandomDist;
	}

	// Keep roughly same altitude with slight random drift
	StrafeLocation.Z = DragonLoc.Z + FMath::FRandRange(-200.f, 200.f);

	UE_LOG(LogTemp, Log, TEXT("AirStrafe: target location (%.0f, %.0f, %.0f)"),
		StrafeLocation.X, StrafeLocation.Y, StrafeLocation.Z);
}

void UDragonAbility_AirStrafe::Tick(float DeltaTime)
{
	if (!OwnerDragon) return;

	UDragonFlightComponent* Flight =
		OwnerDragon->FindComponentByClass<UDragonFlightComponent>();

	if (!Flight) return;

	Flight->SetAirTarget(StrafeLocation);

	// Auto-finish when close enough to strafe target
	float Dist = FVector::Dist(OwnerDragon->GetActorLocation(), StrafeLocation);
	if (Dist < 300.f)
	{
		bFinished = true;
	}
}

bool UDragonAbility_AirStrafe::IsFinished() const
{
	return bFinished;
}

bool UDragonAbility_AirStrafe::CanBeInterrupted() const
{
	return true;
}

void UDragonAbility_AirStrafe::Abort(EDragonInterruptReason Reason)
{
	Super::Abort(Reason);
}