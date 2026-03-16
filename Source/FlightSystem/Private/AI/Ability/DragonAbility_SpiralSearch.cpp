#include "AI/Ability/DragonAbility_SpiralSearch.h"

void UDragonAbility_SpiralSearch::SetSearchLocation(const FVector& InLocation)
{
   // SearchLocation = InLocation;
    SearchLocation = FVector(61613.709893, 3459.903335, 7779.841142);
}

void UDragonAbility_SpiralSearch::Start(
    ADragonBaseAI* InOwner,
    AActor* InTarget)
{
    Super::Start(InOwner, InTarget);

    bReachedSearchArea = false;
    bFinished          = false;
    CurrentRadius      = InitialRadius;
    Angle              = 0.f;
    AbilityTimer       = 0.f;
}
void UDragonAbility_SpiralSearch::Tick(float DeltaTime)
{
    if (!OwnerDragon) return;

    UDragonFlightComponent* Flight =
        OwnerDragon->FindComponentByClass<UDragonFlightComponent>();

    if (!Flight) return;

    FVector DragonLoc = OwnerDragon->GetActorLocation();

    /* ─────────────────────────────────────────────────────────────
       Phase 1 — Fly to search location
    ───────────────────────────────────────────────────────────── */
    if (!bReachedSearchArea)
    {
        FVector AboveSearch = SearchLocation;
        AboveSearch.Z       = SearchLocation.Z + SearchHeight;

        Flight->SetAirTarget(AboveSearch);

        if (FVector::Dist(DragonLoc, AboveSearch) < ArrivalThreshold)
        {
            bReachedSearchArea = true;

            FVector Offset = DragonLoc - SearchLocation;
            Angle          = FMath::Atan2(Offset.Y, Offset.X);
            CurrentRadius  = FMath::Max(Offset.Size2D(), InitialRadius);
        }

        return;
    }

    /* ─────────────────────────────────────────────────────────────
       Phase 2 — Spiral outward
       Only advance angle and grow radius when dragon is close
       to the current spiral target — this way the dragon actually
       follows the spiral path instead of chasing a target that
       runs away from it every tick.
    ───────────────────────────────────────────────────────────── */
    AbilityTimer += DeltaTime;

    // Current spiral target
    FVector SpiralTarget;
    SpiralTarget.X = SearchLocation.X + FMath::Cos(Angle) * CurrentRadius;
    SpiralTarget.Y = SearchLocation.Y + FMath::Sin(Angle) * CurrentRadius;
    SpiralTarget.Z = SearchLocation.Z + SearchHeight;

    float DistToTarget = FVector::Dist2D(DragonLoc, SpiralTarget);

    // Only step forward along spiral when close enough to current point
    if (DistToTarget < SpiralStepThreshold)
    {
        Angle         += SpiralSpeed;          // fixed angle step, not DeltaTime
        CurrentRadius += RadiusGrowth;         // fixed radius step per waypoint
    }

    Flight->SetAirTarget(SpiralTarget);

    if (AbilityTimer >= SearchDuration)
    {
        bFinished = true;
    }
}

bool UDragonAbility_SpiralSearch::IsFinished() const
{
    return bFinished;
}