#pragma once

#include "CoreMinimal.h"
#include "DragonPersonalitySettings.generated.h"

USTRUCT(BlueprintType)
struct FDragonPersonalitySettings
{
	GENERATED_BODY()

	// Decides TO fight — drives Attack + Threatening weights
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Personality")
	float Aggression = 0.5f;

	// Decides to INVESTIGATE — drives Stalking weight
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Personality")
	float Curiosity = 0.5f;

	// Decides HOW LONG to fight — Attack vs Strafe balance, danger feeling
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Personality")
	float Courage = 0.5f;

	// Decides WHERE to fight — Threatening boost in territory, chase limits
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Personality")
	float Territoriality = 0.5f;

	// Decides to WAIT — drives Strafing + Resting weights
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Personality")
	float Patience = 0.5f;

	// Decides HOW to fight — drives Strafing + Threatening, suppresses Attack at close range
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Personality")
	float Intelligence = 0.5f;
};