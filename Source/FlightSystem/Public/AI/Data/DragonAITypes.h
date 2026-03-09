#pragma once

#include "CoreMinimal.h"
#include "DragonAITypes.generated.h"

UENUM(BlueprintType)
enum class EDragonState : uint8
{
	Idle        UMETA(DisplayName="Idle"),
	Roaming     UMETA(DisplayName="Roaming"),
	Observe     UMETA(DisplayName="Observe"),
	Attack      UMETA(DisplayName="Attack"),
	Rest        UMETA(DisplayName="Rest"),
	Flee        UMETA(DisplayName="Flee"),
	Perch       UMETA(DisplayName="Perch")
};

UENUM(BlueprintType)
enum class EDragonAbilityType : uint8
{
	None            UMETA(DisplayName="None"),
	DiveBomb        UMETA(DisplayName="DiveBomb"),
	AirStrafe       UMETA(DisplayName="AirStrafe"),
	GroundStrafe    UMETA(DisplayName="GroundStrafe"),
	FireBreath      UMETA(DisplayName="FireBreath"),
	FireRain        UMETA(DisplayName="FireRain"),
	ClawAttack      UMETA(DisplayName="ClawAttack"),
	WingGust        UMETA(DisplayName="WingGust")
};