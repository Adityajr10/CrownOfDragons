#include "AI/Ability/DragonAbility_IntimidationFly.h"

void UDragonAbility_IntimidationFly::Start(
    ADragonBaseAI* InOwner,
    AActor* InTarget)
{
    Super::Start(InOwner, InTarget);
    AbilityType = EDragonAbilityType::IntimidationFly;
    bReachedAbovePlayer = false;
    bFinished           = false;

    CircleAngle        = 0.f;
    TotalAngleTraveled = 0.f;
    AbilityTimer       = 0.f;
    MontageTimer       = 0.f;
}

void UDragonAbility_IntimidationFly::Tick(float DeltaTime)
{
    if (!OwnerDragon || !TargetActor) return;

    UDragonFlightComponent* Flight =
        OwnerDragon->FindComponentByClass<UDragonFlightComponent>();

    if (!Flight) return;

    FVector DragonLoc = OwnerDragon->GetActorLocation();
    FVector PlayerLoc = TargetActor->GetActorLocation();

    /* Phase 1 — Fly above the player */
    if (!bReachedAbovePlayer)
    {
        FVector AbovePlayer = PlayerLoc;
        AbovePlayer.Z       = PlayerLoc.Z + CircleHeight;

        Flight->SetAirTarget(AbovePlayer);

        if (FVector::Dist(DragonLoc, AbovePlayer) < ArrivalThreshold)
        {
            bReachedAbovePlayer = true;
        }

        return;
    }

    /* Phase 2 — Circle with intimidation montages */
    AbilityTimer += DeltaTime;
    MontageTimer += DeltaTime;

    FVector ToDragon = (DragonLoc - PlayerLoc).GetSafeNormal();
    FVector Side = FVector::CrossProduct(ToDragon, FVector::UpVector);

    FVector CurrentRadial   = (DragonLoc - PlayerLoc);
    CurrentRadial.Z         = 0.f;
    float   CurrentDist     = CurrentRadial.Size();
    float   RadiusCorrection = CircleRadius - CurrentDist;

    FVector CircleTarget    = DragonLoc
                            + Side     * CircleSpeed * 100.f
                            + ToDragon * RadiusCorrection;

    CircleTarget.Z = PlayerLoc.Z + CircleHeight;

    Flight->SetAirTarget(CircleTarget);

    float AngleDelta        = DeltaTime * CircleSpeed;
    TotalAngleTraveled     += AngleDelta;

    /* Phase 3 — Play random intimidation montage periodically */
    if (MontageTimer > MontageInterval && IntimidationMontages.Num() > 0)
    {
        MontageTimer = 0.f;

        int32 Index = FMath::RandRange(0, IntimidationMontages.Num() - 1);
        OwnerDragon->PlayAnimMontage(IntimidationMontages[Index]);
    }

    /* End */
    if (CirclesBeforeFinish > 0)
    {
        if (TotalAngleTraveled >= CirclesBeforeFinish * TWO_PI)
            bFinished = true;
    }
    else
    {
        if (AbilityTimer > IntimidationDuration)
            bFinished = true;
    }
}

bool UDragonAbility_IntimidationFly::IsFinished() const
{
    return bFinished;
}

// Phase 1 (flying to position): interruptible — hasn't started intimidation yet
// Phase 2 (circling + montages): NOT interruptible — mid-performance
bool UDragonAbility_IntimidationFly::CanBeInterrupted() const
{
    return !bReachedAbovePlayer;
}

void UDragonAbility_IntimidationFly::Abort(EDragonInterruptReason Reason)
{
    // Stop any playing intimidation montage
    if (bReachedAbovePlayer && OwnerDragon && IntimidationMontages.Num() > 0)
    {
        OwnerDragon->StopAnimMontage(nullptr); // stops whatever is playing
    }

    Super::Abort(Reason);
}