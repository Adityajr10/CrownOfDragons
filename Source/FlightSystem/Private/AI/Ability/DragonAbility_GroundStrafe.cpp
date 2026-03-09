// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Ability/DragonAbility_GroundStrafe.h"
#include "AI/Ability/DragonAbility_GroundStrafe.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AI/AbilityActor/DragonStrafeFireActor.h"

void UDragonAbility_GroundStrafe::Start(ADragonBaseAI* InOwner, AActor* InTarget)
{
	Super::Start(InOwner, InTarget);

	if (!OwnerDragon || !TargetActor)
	{
		bFinished = true;
		return;
	}

	FVector Dir = OwnerDragon->GetActorForwardVector();

	FVector GroundStart = TargetActor->GetActorLocation();
	GroundStart.Z += OwnerDragon->StrafeHeight;

	Start1 = GroundStart;
	End = GroundStart + Dir * OwnerDragon->StrafeDistance;

	OwnerDragon->SetActorLocation(Start1);
	OwnerDragon->SetActorRotation(Dir.Rotation());

	MoveDir = (End - Start1).GetSafeNormal();

	if (OwnerDragon->StrafeMontage)
		OwnerDragon->PlayAnimMontage(OwnerDragon->StrafeMontage);

	UWorld* World = OwnerDragon->GetWorld();

	if (World && StrafeFireClass)
	{
		FVector SpawnLoc =
			OwnerDragon->GetMesh()->GetSocketLocation("Tongue");

		FireActor = World->SpawnActor<ADragonStrafeFireActor>(
			StrafeFireClass,
			SpawnLoc,
			OwnerDragon->GetActorRotation()
		);

		if (FireActor)
		{
			FireActor->AttachToActor(
				OwnerDragon,
				FAttachmentTransformRules::KeepWorldTransform
			);
		}
	}

	bStarted = true;
}


void UDragonAbility_GroundStrafe::Tick(float DeltaTime)
{
	if (!OwnerDragon || !bStarted) return;

	OwnerDragon->GetCharacterMovement()->SetMovementMode(MOVE_Flying);

	OwnerDragon->GetCharacterMovement()->Velocity =
		MoveDir * OwnerDragon->StrafeSpeed;

	OwnerDragon->SetActorRotation(MoveDir.Rotation());

	if (FVector::Dist(
		OwnerDragon->GetActorLocation(),
		End) < 300.f)
	{
		OwnerDragon->GetCharacterMovement()->StopMovementImmediately();

		if (FireActor)
		{
			FireActor->StopFire();
		}

		bFinished = true;
	}
}