#pragma once

#include "CoreMinimal.h"
#include "AI/Data/DragonAITypes.h"
#include "DragonAbilityData.generated.h"

USTRUCT(BlueprintType)
struct FDragonAbilityData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ability")
	EDragonAbilityType AbilityType = EDragonAbilityType::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ability")
	EDragonInstinct InstinctType = EDragonInstinct::Attacking;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ability")
	float Cooldown = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ability")
	float EnergyCost = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ability")
	float PreferredDistanceMin = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ability")
	float PreferredDistanceMax = 5000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ability")
	float Weight = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ability")
	float MinAltitudeDifference = -10000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ability")
	float MaxAltitudeDifference = 10000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ability")
	float AbortDistanceThreshold = 99999.f;


	// Can this ability be interrupted mid-execution?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ability|Interrupt")
	bool bCanBeInterrupted = true;

	// Force interrupt even if bCanBeInterrupted is false when health below this %
	// 0.0 = never force, 0.15 = force at 15% health
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ability|Interrupt")
	float ForceInterruptHealthPercent = 0.15f;

	// ============================================================
	// NEW: Priority
	// ============================================================

	// Normal = standard pool
	// Reactive = only after damage interrupt (WingGust, Roar as reaction)
	// Desperate = only at low HP (<25%)
	// Efficient = preferred when energy is low
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ability|Priority")
	EDragonAbilityPriority Priority = EDragonAbilityPriority::Normal;

	// ============================================================
	// NEW: Energy efficiency
	// ============================================================

	// Override efficiency score (0 = auto-calculate from EnergyCost)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ability|Energy")
	float EfficiencyOverride = 0.f;

	// Get energy efficiency for scoring (cheap = high, expensive = low)
	float GetEnergyEfficiency(float MaxEnergy) const
	{
		if (EfficiencyOverride > 0.f) return EfficiencyOverride;
		if (MaxEnergy <= 0.f) return 1.f;
		return FMath::Clamp(1.f - (EnergyCost / MaxEnergy), 0.05f, 1.f);
	}

	// Bonus for expensive abilities when energy is full
	float GetSpendBonus(float MaxEnergy) const
	{
		if (MaxEnergy <= 0.f) return 1.f;
		return 1.f + (EnergyCost / MaxEnergy) * 0.5f;
	}
};