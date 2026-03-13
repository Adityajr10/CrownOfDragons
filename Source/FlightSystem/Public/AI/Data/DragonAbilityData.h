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
};