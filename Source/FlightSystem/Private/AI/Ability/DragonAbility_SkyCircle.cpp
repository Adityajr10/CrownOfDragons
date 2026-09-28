#include "AI/Ability/DragonAbility_SkyCircle.h"
#include "FlightSystem/DragonFlightComponent.h"

void UDragonAbility_SkyCircle::Start(ADragonBaseAI* InOwner, AActor* InTarget)
{
	Super::Start(InOwner, InTarget);
	AbilityType = EDragonAbilityType::SkyCircle;
	CircleAngle  = 0.f;
	AbilityTimer = 0.f;
	bFinished    = false;
}

void UDragonAbility_SkyCircle::Tick(float DeltaTime)
{
	if (!OwnerDragon) return;

	UDragonFlightComponent* Flight =
		OwnerDragon->FindComponentByClass<UDragonFlightComponent>();

	if (!Flight) return;

	AbilityTimer += DeltaTime;
	CircleAngle  += DeltaTime * 0.4f;

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

	if (AbilityTimer >= CircleDuration)
	{
		bFinished = true;
	}
}

bool UDragonAbility_SkyCircle::IsFinished() const
{
	return bFinished;
}

// Always interruptible — just flying in circles, no commitment
bool UDragonAbility_SkyCircle::CanBeInterrupted() const
{
	return true;
}

void UDragonAbility_SkyCircle::Abort(EDragonInterruptReason Reason)
{
	// Nothing to clean up — just circling
	Super::Abort(Reason);
}