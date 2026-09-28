#include "AI/Ability/DragonAbility_ThermalGlide.h"

void UDragonAbility_ThermalGlide::Start(
    ADragonBaseAI* InOwner,
    AActor* InTarget)
{
    Super::Start(InOwner, InTarget);
    AbilityType = EDragonAbilityType::ThermalGlide;
    AbilityTimer = 0.f;
    BankTimer    = 0.f;
    bFinished    = false;

    if (!OwnerDragon) return;

    StartAltitude = OwnerDragon->GetActorLocation().Z;
    CurrentYaw    = OwnerDragon->GetActorRotation().Yaw;

    NextBankInterval = FMath::FRandRange(BankIntervalMin, BankIntervalMax);

    CurrentDirection = EGlideDirection::Forward;
    PlayCurrentMontage();
}

void UDragonAbility_ThermalGlide::Tick(float DeltaTime)
{
    if (!OwnerDragon) return;

    UDragonFlightComponent* Flight =
        OwnerDragon->FindComponentByClass<UDragonFlightComponent>();

    if (!Flight) return;

    AbilityTimer += DeltaTime;
    BankTimer    += DeltaTime;

    if (CurrentDirection == EGlideDirection::Left)
    {
        CurrentYaw -= BankTurnRate * DeltaTime;
    }
    else if (CurrentDirection == EGlideDirection::Right)
    {
        CurrentYaw += BankTurnRate * DeltaTime;
    }

    if (BankTimer >= NextBankInterval)
    {
        BankTimer = 0.f;
        NextBankInterval = FMath::FRandRange(BankIntervalMin, BankIntervalMax);
        PickNewDirection();
        PlayCurrentMontage();
    }

    UAnimMontage* ActiveMontage = nullptr;

    if (CurrentDirection == EGlideDirection::Forward)
        ActiveMontage = GlideForwardMontage;
    else if (CurrentDirection == EGlideDirection::Left)
        ActiveMontage = GlideLeftMontage;
    else if (CurrentDirection == EGlideDirection::Right)
        ActiveMontage = GlideRightMontage;

    if (ActiveMontage)
    {
        UAnimInstance* Anim = OwnerDragon->GetMesh()->GetAnimInstance();

        if (Anim && !Anim->Montage_IsPlaying(ActiveMontage))
        {
            OwnerDragon->PlayAnimMontage(ActiveMontage);
        }
    }

    FVector Forward = FVector(
        FMath::Cos(FMath::DegreesToRadians(CurrentYaw)),
        FMath::Sin(FMath::DegreesToRadians(CurrentYaw)),
        0.f
    );

    FVector DragonLoc = OwnerDragon->GetActorLocation();
    FVector GlideTarget = DragonLoc + Forward * GlideLeadDistance;

    GlideTarget.Z = StartAltitude +
                    AltitudeDriftAmplitude *
                    FMath::Sin(AbilityTimer * AltitudeDriftSpeed);

    Flight->SetAirTarget(GlideTarget);

    if (AbilityTimer >= GlideDuration)
    {
        if (ActiveMontage)
            OwnerDragon->StopAnimMontage(ActiveMontage);

        bFinished = true;
    }
}

void UDragonAbility_ThermalGlide::PickNewDirection()
{
    int32 Roll = FMath::RandRange(0, 3);

    if (Roll == 0)
        CurrentDirection = EGlideDirection::Left;
    else if (Roll == 1)
        CurrentDirection = EGlideDirection::Right;
    else
        CurrentDirection = EGlideDirection::Forward;
}

void UDragonAbility_ThermalGlide::PlayCurrentMontage()
{
    UAnimMontage* Montage = nullptr;

    if (CurrentDirection == EGlideDirection::Forward)
        Montage = GlideForwardMontage;
    else if (CurrentDirection == EGlideDirection::Left)
        Montage = GlideLeftMontage;
    else if (CurrentDirection == EGlideDirection::Right)
        Montage = GlideRightMontage;

    if (Montage)
        OwnerDragon->PlayAnimMontage(Montage);
}

bool UDragonAbility_ThermalGlide::IsFinished() const
{
    return bFinished;
}

// Always interruptible — just gentle flying, no commitment at all
bool UDragonAbility_ThermalGlide::CanBeInterrupted() const
{
    return true;
}

void UDragonAbility_ThermalGlide::Abort(EDragonInterruptReason Reason)
{
    // Stop whichever glide montage is currently playing
    if (OwnerDragon)
    {
        UAnimMontage* ActiveMontage = nullptr;

        if (CurrentDirection == EGlideDirection::Forward)
            ActiveMontage = GlideForwardMontage;
        else if (CurrentDirection == EGlideDirection::Left)
            ActiveMontage = GlideLeftMontage;
        else if (CurrentDirection == EGlideDirection::Right)
            ActiveMontage = GlideRightMontage;

        if (ActiveMontage)
            OwnerDragon->StopAnimMontage(ActiveMontage);
    }

    Super::Abort(Reason);
}