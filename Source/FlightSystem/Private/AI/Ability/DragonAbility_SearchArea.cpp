#include "AI/Ability/DragonAbility_SearchArea.h"

void UDragonAbility_SearchArea::SetSearchLocation(const FVector& InLocation)
{
    //SearchLocation = InLocation;
    SearchLocation= FVector(36988.972731, 22267.368020, 7439.448210);
}

void UDragonAbility_SearchArea::Start(
    ADragonBaseAI* InOwner,
    AActor* InTarget)
{
    Super::Start(InOwner, InTarget);
    AbilityType = EDragonAbilityType::SearchArea;
    bReachedSearchArea = false;
    bFinished          = false;
    CircleAngle        = 0.f;
    AbilityTimer       = 0.f;
}

void UDragonAbility_SearchArea::Tick(float DeltaTime)
{
    if (!OwnerDragon) return;

    UDragonFlightComponent* Flight =
        OwnerDragon->FindComponentByClass<UDragonFlightComponent>();

    if (!Flight) return;

    FVector DragonLoc = OwnerDragon->GetActorLocation();

    if (!bReachedSearchArea)
    {
        FVector AboveSearch = SearchLocation;
        AboveSearch.Z       = SearchLocation.Z + SearchHeight;

        Flight->SetAirTarget(AboveSearch);

        if (FVector::Dist(DragonLoc, AboveSearch) < ArrivalThreshold)
        {
            bReachedSearchArea = true;
            FVector Offset = DragonLoc - SearchLocation;
            CircleAngle    = FMath::Atan2(Offset.Y, Offset.X);
        }

        return;
    }

    AbilityTimer += DeltaTime;
    CircleAngle  += DeltaTime * CircleSpeed;

    FVector CircleTarget;
    CircleTarget.X = SearchLocation.X + FMath::Cos(CircleAngle) * SearchRadius;
    CircleTarget.Y = SearchLocation.Y + FMath::Sin(CircleAngle) * SearchRadius;
    CircleTarget.Z = SearchLocation.Z + SearchHeight;

    Flight->SetAirTarget(CircleTarget);

    if (AbilityTimer >= SearchDuration)
    {
        bFinished = true;
    }
}

bool UDragonAbility_SearchArea::IsFinished() const
{
    return bFinished;
}

// Always interruptible — just searching, no commitment
bool UDragonAbility_SearchArea::CanBeInterrupted() const
{
    return true;
}

void UDragonAbility_SearchArea::Abort(EDragonInterruptReason Reason)
{
    Super::Abort(Reason);
}