#include "AI/Ability/DragonAbility_WingGust.h"
#include "AI/AbilityActor/DragonWingGustActor.h"

void UDragonAbility_WingGust::Start(ADragonBaseAI* InOwner, AActor* InTarget)
{
	Super::Start(InOwner, InTarget);
	AbilityType = EDragonAbilityType::WingGust;
	bMontageStarted = false;
	bSpawned = false;
	bFinished = false;
	Timer = 0.f;
}

void UDragonAbility_WingGust::Tick(float DeltaTime)
{
	if (!OwnerDragon || !TargetActor) return;

	UDragonFlightComponent* Flight =
		OwnerDragon->FindComponentByClass<UDragonFlightComponent>();

	if (!Flight) return;

	FVector DragonLoc = OwnerDragon->GetActorLocation();
	FVector PlayerLoc = TargetActor->GetActorLocation();

	float Dist = FVector::Dist(DragonLoc, PlayerLoc);

	/* Move toward player */

	if (!bMontageStarted && Dist > AttackDistance)
	{
		Flight->SetAirTarget(PlayerLoc);
		return;
	}

	/* Start montage */
	/* Start montage but keep movement */

	if (!bMontageStarted)
	{
		bMontageStarted = true;

		if (WingGustMontage)
		{
			OwnerDragon->PlayAnimMontage(WingGustMontage);
		}
	}

	/* Keep flying forward while montage plays */

	FVector DragonLoc1 = OwnerDragon->GetActorLocation();
	FVector Forward = OwnerDragon->GetActorForwardVector();

	FVector MoveTarget =
		DragonLoc1 + Forward * 800.f;

	Flight->SetAirTarget(MoveTarget);

	/* Wait delay before spawning gust */

	Timer += DeltaTime;

	if (!bSpawned && Timer >= SpawnDelay)
	{
		bSpawned = true;

		if (!WingGustActorClass) return;

		FVector SpawnLoc = OwnerDragon->GetActorLocation();

		FVector TargetLoc =
			SpawnLoc + OwnerDragon->GetActorForwardVector() * GustDistance;

		UWorld* World = OwnerDragon->GetWorld();

		if (!World) return;

		ADragonWingGustActor* Gust =
			World->SpawnActor<ADragonWingGustActor>(
				WingGustActorClass,
				SpawnLoc,
				OwnerDragon->GetActorRotation()
			);

		if (Gust)
		{
			Gust->Init(TargetLoc);
		}
	}

	/* Finish ability */

	if (bSpawned)
	{
		bFinished = true;
	}
}

bool UDragonAbility_WingGust::IsFinished() const
{
	//UE_LOG(LogTemp, Log,     TEXT("WingGustFinish!"));
	return bFinished;
	
}