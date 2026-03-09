// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Ability/DragonFireProjectile.h"
#include "AI/AbilityActor/DragonFireActor.h"

/*void UDragonFireProjectile::Start(ADragonBaseAI* InOwner, AActor* InTarget)
{
	Super::Start(InOwner, InTarget);

	SpawnTimer = 0.f;
	SpawnedCount = 0;
}*/

void UDragonFireProjectile::Start(ADragonBaseAI* InOwner, AActor* InTarget)
{
    Super::Start(InOwner, InTarget);

    MoveTarget = FVector::ZeroVector;
    bReachedPoint = false;
    bFired = false;
}

void UDragonFireProjectile::Tick(float DeltaTime)
{
    if (!OwnerDragon || !TargetActor) return;

    UDragonFlightComponent* Flight =
        OwnerDragon->FindComponentByClass<UDragonFlightComponent>();

    if (!Flight) return;

    /* ---------- PHASE 1 : MOVE TO POINT ---------- */

    if (!bReachedPoint)
    {
        if (MoveTarget.IsZero())
        {
            FVector PlayerLoc = TargetActor->GetActorLocation();

            FVector Offset;
            Offset.X = FMath::FRandRange(-MoveRadius, MoveRadius);
            Offset.Y = FMath::FRandRange(-MoveRadius, MoveRadius);
            Offset.Z = MoveHeight;

            MoveTarget = PlayerLoc + Offset;
        }

        Flight->SetAirTarget(MoveTarget);

        float Dist =
            FVector::Dist(MoveTarget, OwnerDragon->GetActorLocation());

        if (Dist < 350.f)
        {
            bReachedPoint = true;
        }

        return;
    }

    /* ---------- PHASE 2 : ROTATE TOWARD PLAYER ---------- */

    FVector DragonLoc = OwnerDragon->GetActorLocation();
    FVector PlayerLoc = TargetActor->GetActorLocation();

    FRotator CurrentRot = OwnerDragon->GetActorRotation();
    FRotator TargetRot = (PlayerLoc - DragonLoc).Rotation();

    FRotator NewRot =
        FMath::RInterpTo(CurrentRot, TargetRot, DeltaTime, 3.f);

    OwnerDragon->SetActorRotation(NewRot);

    float Angle =
        FMath::Abs((TargetRot - NewRot).Yaw);

    if (Angle > 5.f)
    {
        return;
    }

    /* ---------- PHASE 3 : FIRE ---------- */

    if (!bFired)
    {
        bFired = true;

        if (!FireActorClass) return;

        FVector SpawnLoc =
            OwnerDragon->GetMesh()->GetSocketLocation("Tongue");

        FVector Dir =
            (PlayerLoc - SpawnLoc).GetSafeNormal();

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

        bFinished = true;
    }
}
