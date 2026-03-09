#pragma once

#include "CoreMinimal.h"
#include "DragonPersonalitySettings.generated.h"

USTRUCT(BlueprintType)
struct FDragonPersonalitySettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Personality")
	float Aggression = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Personality")
	float Curiosity = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Personality")
	float Courage = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Personality")
	float Territoriality = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Personality")
	float Patience = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Personality")
	float Intelligence = 0.5f;
};