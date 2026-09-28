// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Ability/DragonAbility_Roar.h"

void UDragonAbility_Roar::Start(ADragonBaseAI* InOwner, AActor* InTarget)
{
	Super::Start(InOwner, InTarget);
AbilityType = EDragonAbilityType::Roar;
	bRoarStarted = false;

	if (RoarMontages.Num() > 0)
	{
		int32 Index = FMath::RandRange(0, RoarMontages.Num() - 1);
		CurrentMontage = RoarMontages[Index];
	}
}

void UDragonAbility_Roar::Tick(float DeltaTime)
{
	if (!OwnerDragon || !TargetActor) return;

	/* Face the player */
	FVector DragonLoc = OwnerDragon->GetActorLocation();
	FVector PlayerLoc = TargetActor->GetActorLocation();

	FRotator CurrentRot = OwnerDragon->GetActorRotation();
	FRotator TargetRot = (PlayerLoc - DragonLoc).Rotation();

	FRotator NewRot =
		FMath::RInterpTo(CurrentRot, TargetRot, DeltaTime, RotationSpeed);

	OwnerDragon->SetActorRotation(NewRot);

	/* Start roar once facing player */
	if (!bRoarStarted)
	{
		bRoarStarted = true;

		if (CurrentMontage)
		{
			OwnerDragon->PlayAnimMontage(CurrentMontage);
		}
	}
	
	if (CurrentMontage &&
		!OwnerDragon->GetMesh()->GetAnimInstance()->Montage_IsPlaying(CurrentMontage))
	{
		bFinished = true;
	}
}

bool UDragonAbility_Roar::IsFinished() const
{
	return bFinished;
}