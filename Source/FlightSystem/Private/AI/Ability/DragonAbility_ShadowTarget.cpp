#include "AI/Ability/DragonAbility_ShadowTarget.h"

void UDragonAbility_ShadowTarget::Start(
    ADragonBaseAI* InOwner,
    AActor* InTarget)
{
    Super::Start(InOwner, InTarget);
    AbilityType = EDragonAbilityType::ShadowTarget;
    bReachedAbovePlayer = false;
    bFinished           = false;
    AbilityTimer        = 0.f;
}

void UDragonAbility_ShadowTarget::Tick(float DeltaTime)
{
    if (!OwnerDragon || !TargetActor) return;

    UDragonFlightComponent* Flight =
        OwnerDragon->FindComponentByClass<UDragonFlightComponent>();

    if (!Flight) return;

    FVector DragonLoc = OwnerDragon->GetActorLocation();
    FVector PlayerLoc = TargetActor->GetActorLocation();

    FVector AbovePlayer = PlayerLoc;
    AbovePlayer.Z       = PlayerLoc.Z + ShadowHeight;

    /* ─────────────────────────────────────────────────────────────
       Phase 1 — Move above player
    ───────────────────────────────────────────────────────────── */
    if (!bReachedAbovePlayer)
    {
        Flight->SetAirTarget(AbovePlayer);

        if (FVector::Dist(DragonLoc, AbovePlayer) < ArrivalThreshold)
        {
            bReachedAbovePlayer = true;
        }

        return;
    }

    /* ─────────────────────────────────────────────────────────────
       Phase 2 — Follow player for FollowDuration seconds
    ───────────────────────────────────────────────────────────── */
    AbilityTimer += DeltaTime;

    Flight->SetAirTarget(AbovePlayer);

    /* ─────────────────────────────────────────────────────────────
       Phase 3 — Only rotate YAW toward player
       Pitch and roll are left alone so flight component
       does not fight against SetActorRotation every tick.
    ───────────────────────────────────────────────────────────── */
    FRotator CurrentRot = OwnerDragon->GetActorRotation();

    // Get yaw-only direction toward player
    FVector ToPlayer        = (PlayerLoc - DragonLoc).GetSafeNormal();
    float   TargetYaw       = FMath::RadiansToDegrees(
                                FMath::Atan2(ToPlayer.Y, ToPlayer.X));

    float SmoothedYaw = FMath::FInterpTo(
                        CurrentRot.Yaw,
                        TargetYaw,
                        DeltaTime,
                        RotationSpeed);
    // Only apply yaw — leave pitch and roll untouched
    OwnerDragon->SetActorRotation(
        FRotator(CurrentRot.Pitch, SmoothedYaw, CurrentRot.Roll));

    /* ─────────────────────────────────────────────────────────────
       End
    ───────────────────────────────────────────────────────────── */
    if (AbilityTimer >= FollowDuration)
    {
        bFinished = true;
    }
}

bool UDragonAbility_ShadowTarget::IsFinished() const
{
    return bFinished;
}