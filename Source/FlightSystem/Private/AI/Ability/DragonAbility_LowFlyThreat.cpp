#include "AI/Ability/DragonAbility_LowFlyThreat.h"

void UDragonAbility_LowFlyThreat::Start(
    ADragonBaseAI* InOwner,
    AActor* InTarget)
{
    Super::Start(InOwner, InTarget);
    AbilityType = EDragonAbilityType::LowFlyThreat;
    bThreatStarted  = false;
    bFlyPastStarted = false;
    bExitStarted    = false;
    bFinished       = false;

    ApproachTarget  = FVector::ZeroVector;
    FlyPastTarget   = FVector::ZeroVector;
    ExitTarget      = FVector::ZeroVector;
}

void UDragonAbility_LowFlyThreat::Tick(float DeltaTime)
{
    if (!OwnerDragon || !TargetActor) return;

    UDragonFlightComponent* Flight =
        OwnerDragon->FindComponentByClass<UDragonFlightComponent>();

    if (!Flight) return;

    FVector DragonLoc = OwnerDragon->GetActorLocation();
    FVector PlayerLoc = TargetActor->GetActorLocation();

    FVector ToPlayer   = (PlayerLoc - DragonLoc).GetSafeNormal();
    FVector FromPlayer = -ToPlayer;

    /* Phase 1 — Approach */
    if (!bThreatStarted)
    {
        ApproachTarget   = PlayerLoc + FromPlayer * ThreatDistance;
        ApproachTarget.Z = PlayerLoc.Z + LowHeight;

        float Dist = FVector::Dist(DragonLoc, ApproachTarget);

        if (Dist > 200.f)
        {
            Flight->SetAirTarget(ApproachTarget);
            return;
        }

        bThreatStarted = true;

        if (LowFlyThreatMontage)
            OwnerDragon->PlayAnimMontage(LowFlyThreatMontage);

        return;
    }

    /* Phase 2 — Wait for montage */
    if (!bFlyPastStarted)
    {
        UAnimInstance* Anim = OwnerDragon->GetMesh()->GetAnimInstance();

        if (Anim && LowFlyThreatMontage &&
            Anim->Montage_IsPlaying(LowFlyThreatMontage))
        {
            return;
        }

        bFlyPastStarted = true;

        FVector Side =
            FVector::CrossProduct(ToPlayer, FVector::UpVector).GetSafeNormal();

        int32 Choice = FMath::RandRange(0, 2);

        switch (Choice)
        {
            case 0:
            {
                FlyPastTarget = DragonLoc
                              + Side            * -800.f
                              + ToPlayer        *  400.f;
                FlyPastTarget.Z = PlayerLoc.Z + LowHeight;
                break;
            }
            case 1:
            {
                FlyPastTarget = DragonLoc
                              + Side            *  800.f
                              + ToPlayer        *  400.f;
                FlyPastTarget.Z = PlayerLoc.Z + LowHeight;
                break;
            }
            case 2:
            {
                FlyPastTarget = DragonLoc
                              + ToPlayer        *  400.f
                              + FVector(0, 0, 800.f);
                break;
            }
        }
    }

    /* Phase 3 — Fly to exit point */
    Flight->SetAirTarget(FlyPastTarget);

    float DistToTarget = FVector::Dist(DragonLoc, FlyPastTarget);

    if (DistToTarget < 250.f)
    {
        bFinished = true;
    }
}

bool UDragonAbility_LowFlyThreat::IsFinished() const
{
    return bFinished;
}

// Phase 1 (approaching player): YES — hasn't started the threat yet
// Phase 2 (montage playing): NO — mid-roar/threat animation
// Phase 3 (flying past): YES — just flying to exit, safe to abort
bool UDragonAbility_LowFlyThreat::CanBeInterrupted() const
{
    if (!bThreatStarted)
    {
        // Phase 1: approaching, can abort
        return true;
    }

    if (!bFlyPastStarted)
    {
        // Phase 2: montage playing, committed
        return false;
    }

    // Phase 3: flying to exit, can abort
    return true;
}

void UDragonAbility_LowFlyThreat::Abort(EDragonInterruptReason Reason)
{
    if (OwnerDragon)
    {
        // If montage is playing (phase 2 force-interrupt via health override), stop it
        if (bThreatStarted && !bFlyPastStarted && LowFlyThreatMontage)
        {
            OwnerDragon->StopAnimMontage(LowFlyThreatMontage);
            UE_LOG(LogTemp, Log, TEXT("LowFlyThreat: force-aborted during montage"));
        }
    }

    Super::Abort(Reason);
}