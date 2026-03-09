// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Ability/DragonAbility_PerchAndWatch.h"
#include "GameFramework/CharacterMovementComponent.h"

void UDragonAbility_PerchAndWatch::Start(ADragonBaseAI* InOwner, AActor* InTarget)
{
	Super::Start(InOwner, InTarget);

	if (!OwnerDragon) return;

	UE_LOG(LogTemp, Warning, TEXT("🐉 PERCH ABILITY STARTED"));

	FVector Origin = OwnerDragon->StartLocation;

	FVector RandomXY;
	RandomXY.X = FMath::FRandRange(Origin.X - 15000.f, Origin.X + 15000.f);
	RandomXY.Y = FMath::FRandRange(Origin.Y - 15000.f, Origin.Y + 15000.f);
	RandomXY.Z = Origin.Z + 8000.f;

	FHitResult Hit;
	FVector TraceStart = RandomXY;
	FVector TraceEnd = RandomXY - FVector(0,0,20000);

	bool bHit = OwnerDragon->GetWorld()->LineTraceSingleByChannel(
		Hit,
		TraceStart,
		TraceEnd,
		ECC_Visibility
	);

	if(!bHit)
	{
		UE_LOG(LogTemp, Warning, TEXT("No perch point found"));
		bFinished = true;
		return;
	}

	PerchLocation = Hit.ImpactPoint;
	PerchLocation.Z += 150.f;

	UDragonFlightComponent* Flight =
		OwnerDragon->FindComponentByClass<UDragonFlightComponent>();

	if(!Flight)
	{
		bFinished = true;
		return;
	}

	OwnerDragon->GetCharacterMovement()->MaxFlySpeed = 3000.f;
	OwnerDragon->GetCharacterMovement()->BrakingDecelerationFlying = 5000.f;

	Flight->SetAirTarget(PerchLocation);

	bLandingStarted = true;
}

void UDragonAbility_PerchAndWatch::Tick(float DeltaTime)
{
	if (!OwnerDragon || !bLandingStarted) return;

	FVector Loc = OwnerDragon->GetActorLocation();

	float ZDiff = Loc.Z - PerchLocation.Z;

	if (ZDiff > 180.f)
	{
		float GlideSpeed = 800.f;

		Loc.Z -= GlideSpeed * DeltaTime;

		OwnerDragon->SetActorLocation(Loc);

		return;
	}

	if (!bLanded)
	{
		bLanded = true;

		Loc.Z = PerchLocation.Z;
		OwnerDragon->SetActorLocation(Loc);

		OwnerDragon->IsFlying = false;

		UE_LOG(LogTemp, Warning, TEXT("🐉 Dragon LANDED"));

		if (OwnerDragon->WingFoldMontage)
		{
			OwnerDragon->PlayAnimMontage(
				OwnerDragon->WingFoldMontage
			);
		}

		FTimerHandle RoarTimer;

		OwnerDragon->GetWorldTimerManager().SetTimer(
			RoarTimer,
			[this]()
			{
				if (!OwnerDragon->IsFlying && OwnerDragon->PerchRoarMontage)
				{
					OwnerDragon->PlayAnimMontage(
						OwnerDragon->PerchRoarMontage
					);
				}
			},
			3.f,
			true
		);

		FTimerHandle TakeoffTimer;

		OwnerDragon->GetWorldTimerManager().SetTimer(
			TakeoffTimer,
			[this]()
			{
				if (!OwnerDragon->IsFlying)
				{
					OwnerDragon->TakeoffFromPerch();
				}

				bFinished = true;
			},
			OwnerDragon->PerchWatchTime,
			false
		);
	}
}
