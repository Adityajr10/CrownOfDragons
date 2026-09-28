#include "AI/Ability/DragonAbility_PlayerOrbitStrafe.h"
#include "Kismet/GameplayStatics.h"

void UDragonAbility_PlayerOrbitStrafe::Start(ADragonBaseAI* InOwner, AActor* InTarget)
{
	Super::Start(InOwner, InTarget);

	if (!OwnerDragon)
	{
		bFinished = true;
		return;
	}

	FlightComp = OwnerDragon->FindComponentByClass<UDragonFlightComponent>();
	FireComp = OwnerDragon->FindComponentByClass<UDragonFireBreathComponent>();

	if (!FlightComp)
	{
		bFinished = true;
		return;
	}

	if (FireComp && TargetActor)
	{
		FireComp->StartFire(TargetActor);
	}

	UE_LOG(LogTemp, Warning, TEXT("ORBIT STRAFE STARTED"));
}

void UDragonAbility_PlayerOrbitStrafe::Tick(float DeltaTime)
{
	if (!OwnerDragon || !TargetActor || !FlightComp)
	{
		bFinished = true;
		return;
	}

	FVector DragonLoc = OwnerDragon->GetActorLocation();
	FVector PlayerLoc = TargetActor->GetActorLocation();

	FVector ToDragon = (DragonLoc - PlayerLoc).GetSafeNormal();
	FVector Side = FVector::CrossProduct(ToDragon, FVector::UpVector);

	FVector Target;
	Target = PlayerLoc + Side * OrbitRadius;
	Target.Z += OrbitHeight;

	FlightComp->SetAirTarget(Target);
}

void UDragonAbility_PlayerOrbitStrafe::End()
{
	if (FireComp)
	{
		FireComp->StopFire();
	}
}

bool UDragonAbility_PlayerOrbitStrafe::IsFinished() const
{
	return bFinished;
}

// Always interruptible — orbiting is not a committed action
// The dragon can break off the strafe at any point
bool UDragonAbility_PlayerOrbitStrafe::CanBeInterrupted() const
{
	return true;
}

void UDragonAbility_PlayerOrbitStrafe::Abort(EDragonInterruptReason Reason)
{
	// CRITICAL: must stop fire breath or it keeps going after ability ends
	if (FireComp)
	{
		FireComp->StopFire();
	}

	UE_LOG(LogTemp, Log, TEXT("OrbitStrafe aborted — fire stopped"));

	Super::Abort(Reason);
}