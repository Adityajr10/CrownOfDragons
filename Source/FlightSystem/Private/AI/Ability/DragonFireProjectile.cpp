// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Ability/DragonFireProjectile.h"
#include "AI/AbilityActor/DragonFireActor.h"

void UDragonFireProjectile::Start(ADragonBaseAI* InOwner, AActor* InTarget)
{
	Super::Start(InOwner, InTarget);

	SpawnTimer = 0.f;
	SpawnedCount = 0;
}

void UDragonFireProjectile::Tick(float DeltaTime)
{
	if (!OwnerDragon || !TargetActor) return;

	SpawnTimer += DeltaTime;

	UDragonFlightComponent* Flight =
		OwnerDragon->FindComponentByClass<UDragonFlightComponent>();

	if (!Flight) return;

	// Generate new movement target only when needed
	if (!bHasMoveTarget)
	{
		FVector PlayerLoc = TargetActor->GetActorLocation();

		FVector Offset;
		Offset.X = FMath::FRandRange(-MoveRadius, MoveRadius);
		Offset.Y = FMath::FRandRange(-MoveRadius, MoveRadius);
		Offset.Z = MoveHeight;

		CurrentMoveTarget = PlayerLoc + Offset;

		bHasMoveTarget = true;
	}
	
	Flight->SetAirTarget(CurrentMoveTarget);

	if (SpawnTimer < SpawnInterval) return;

	SpawnTimer = 0.f;

	if (!FireActorClass) return;

	FVector SpawnLoc =
		OwnerDragon->GetMesh()->GetSocketLocation("Tongue");

	FVector Dir =
		(TargetActor->GetActorLocation() - SpawnLoc).GetSafeNormal();

	FRotator Rot = Dir.Rotation();

	ADragonFireActor* Fire =
		OwnerDragon->GetWorld()->SpawnActor<ADragonFireActor>(
			FireActorClass,
			SpawnLoc,
			Rot
		);

	if (Fire)
	{
		Fire->Init(Dir);
	}

	SpawnedCount++;
	
	bHasMoveTarget = false;

	if (SpawnedCount >= MaxSpawns)
	{
		bFinished = true;
	}
}
