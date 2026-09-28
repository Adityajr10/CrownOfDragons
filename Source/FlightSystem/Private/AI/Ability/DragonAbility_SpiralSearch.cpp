#include "AI/Ability/DragonAbility_SpiralSearch.h"

void UDragonAbility_SpiralSearch::SetSearchLocation(const FVector& InLocation)
{
   // SearchLocation = InLocation;
    SearchLocation= FVector(36988.972731, 22267.368020, 7439.448210);
}

void UDragonAbility_SpiralSearch::Start(
    ADragonBaseAI* InOwner,
    AActor* InTarget)
{
    Super::Start(InOwner, InTarget);
    AbilityType = EDragonAbilityType::SpiralSearch;
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

    /* Phase 1 — Fly to search location */
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

    /* Phase 2 — Spiral outward */
    AbilityTimer += DeltaTime;

    FVector SpiralTarget;
    SpiralTarget.X = SearchLocation.X + FMath::Cos(Angle) * CurrentRadius;
    SpiralTarget.Y = SearchLocation.Y + FMath::Sin(Angle) * CurrentRadius;
    SpiralTarget.Z = SearchLocation.Z + SearchHeight;

    float DistToTarget = FVector::Dist2D(DragonLoc, SpiralTarget);

    if (DistToTarget < SpiralStepThreshold)
    {
        Angle         += SpiralSpeed;
        CurrentRadius += RadiusGrowth;
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

// Always interruptible — just searching
bool UDragonAbility_SpiralSearch::CanBeInterrupted() const
{
    return true;
}

void UDragonAbility_SpiralSearch::Abort(EDragonInterruptReason Reason)
{
    Super::Abort(Reason);
}