#include "AI/Ability/DragonAbility_GroundStrafe.h"

void UDragonAbility_GroundStrafe::Start(ADragonBaseAI* InOwner, AActor* InTarget)
{
    Super::Start(InOwner, InTarget);
    AbilityType = EDragonAbilityType::GroundStrafe;
    StrafeTimer = 0.f;

    if (!OwnerDragon) return;

    FVector DragonLoc = OwnerDragon->GetActorLocation();

    if (TargetActor)
    {
        FVector PlayerLoc = TargetActor->GetActorLocation();

        // Pick a random angle AROUND the player (full 360)
        // The point is at StrafeRadius distance FROM the player
        // so the dragon circles around at a safe distance
        float RandomAngle = FMath::FRandRange(0.f, 360.f);
        FVector RandomDir = FVector(
            FMath::Cos(FMath::DegreesToRadians(RandomAngle)),
            FMath::Sin(FMath::DegreesToRadians(RandomAngle)),
            0.f
        );

        float RandomDist = FMath::FRandRange(StrafeRadiusMin, StrafeRadiusMax);

        // Point is AROUND the player, not near the player
        StrafeLocation = PlayerLoc + RandomDir * RandomDist;

        // Low altitude — close to player height but not on the ground
        StrafeLocation.Z = PlayerLoc.Z + FMath::FRandRange(LowHeightMin, LowHeightMax);
    }
    else
    {
        float RandomAngle = FMath::FRandRange(0.f, 360.f);
        FVector RandomDir = FVector(
            FMath::Cos(FMath::DegreesToRadians(RandomAngle)),
            FMath::Sin(FMath::DegreesToRadians(RandomAngle)),
            0.f
        );

        float RandomDist = FMath::FRandRange(StrafeRadiusMin, StrafeRadiusMax);
        StrafeLocation = DragonLoc + RandomDir * RandomDist;
        StrafeLocation.Z = DragonLoc.Z + FMath::FRandRange(-100.f, 100.f);
    }

    UE_LOG(LogTemp, Log, TEXT("GroundStrafe: target location (%.0f, %.0f, %.0f) dist from player=%.0f"),
        StrafeLocation.X, StrafeLocation.Y, StrafeLocation.Z,
        TargetActor ? FVector::Dist(StrafeLocation, TargetActor->GetActorLocation()) : 0.f);
}

void UDragonAbility_GroundStrafe::Tick(float DeltaTime)
{
    if (!OwnerDragon) return;

    UDragonFlightComponent* Flight =
        OwnerDragon->FindComponentByClass<UDragonFlightComponent>();

    if (!Flight) return;

    StrafeTimer += DeltaTime;

    Flight->SetAirTarget(StrafeLocation);

    // Only finish after minimum duration AND close enough
    if (StrafeTimer >= MinStrafeDuration)
    {
        float Dist = FVector::Dist(
            OwnerDragon->GetActorLocation(), StrafeLocation);

        if (Dist < 300.f)
        {
            bFinished = true;
        }
    }
}

bool UDragonAbility_GroundStrafe::IsFinished() const
{
    return bFinished;
}