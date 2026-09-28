#pragma once

#include "CoreMinimal.h"
#include "DragonAbility.h"
#include "DragonAbility_ThermalGlide.generated.h"

class UAnimMontage;

UENUM()
enum class EGlideDirection : uint8
{
    Forward,
    Left,
    Right
};

UCLASS()
class FLIGHTSYSTEM_API UDragonAbility_ThermalGlide : public UDragonAbility
{
    GENERATED_BODY()

public:

    virtual void Start(ADragonBaseAI* InOwner, AActor* InTarget) override;
    virtual void Tick(float DeltaTime) override;
    virtual bool IsFinished() const override;
    
    virtual bool CanBeInterrupted() const override;
    virtual void Abort(EDragonInterruptReason Reason) override;

protected:

    UPROPERTY(EditDefaultsOnly, Category="ThermalGlide")
    UAnimMontage* GlideForwardMontage;

    UPROPERTY(EditDefaultsOnly, Category="ThermalGlide")
    UAnimMontage* GlideLeftMontage;

    UPROPERTY(EditDefaultsOnly, Category="ThermalGlide")
    UAnimMontage* GlideRightMontage;

    // How long the full glide lasts
    UPROPERTY(EditDefaultsOnly, Category="ThermalGlide")
    float GlideDuration = 6.f;

    // How far ahead target is set each tick
    UPROPERTY(EditDefaultsOnly, Category="ThermalGlide")
    float GlideLeadDistance = 600.f;

    // How many degrees per second dragon turns when banking
    UPROPERTY(EditDefaultsOnly, Category="ThermalGlide")
    float BankTurnRate = 30.f;

    // Min and max time before picking a new glide direction
    UPROPERTY(EditDefaultsOnly, Category="ThermalGlide")
    float BankIntervalMin = 1.5f;

    UPROPERTY(EditDefaultsOnly, Category="ThermalGlide")
    float BankIntervalMax = 2.5f;

    // Subtle altitude drift
    UPROPERTY(EditDefaultsOnly, Category="ThermalGlide")
    float AltitudeDriftAmplitude = 40.f;

    UPROPERTY(EditDefaultsOnly, Category="ThermalGlide")
    float AltitudeDriftSpeed = 0.6f;

    float StartAltitude    = 0.f;
    float AbilityTimer     = 0.f;
    float BankTimer        = 0.f;
    float NextBankInterval = 0.f;
    float CurrentYaw       = 0.f;

    EGlideDirection CurrentDirection = EGlideDirection::Forward;

    bool bFinished = false;

    void PickNewDirection();
    void PlayCurrentMontage();
};