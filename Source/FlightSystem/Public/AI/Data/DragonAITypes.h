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

	//ATTACK ABILITIES 
	DiveBomb        UMETA(DisplayName="DiveBomb"),
	AirStrafe       UMETA(DisplayName="AirStrafe"),
	GroundStrafe    UMETA(DisplayName="GroundStrafe"),
	FireBreath      UMETA(DisplayName="FireBreath"),
	FireRain        UMETA(DisplayName="FireRain"),
	ClawAttack      UMETA(DisplayName="ClawAttack"),
	WingGust        UMETA(DisplayName="WingGust"),

	//THREATENING
	Roar            UMETA(DisplayName="Roar"),
	LowFlyThreat    UMETA(DisplayName="LowFlyThreat"),
	IntimidationFly UMETA(DisplayName="IntimidationFly"),

	//  STALKING
	CirclePlayer    UMETA(DisplayName="CirclePlayer"),
	SkyCircle       UMETA(DisplayName="SkyCircle"),
	ShadowTarget    UMETA(DisplayName="ShadowTarget"),

	// ROAMING
	FlyRandom       UMETA(DisplayName="FlyRandom"),
	ThermalGlide    UMETA(DisplayName="ThermalGlide"),
	PerchOnCliff    UMETA(DisplayName="PerchOnCliff"),

	// SEARCHING
	SearchArea      UMETA(DisplayName="SearchArea"),
	InvestigateLastKnownLocation UMETA(DisplayName="InvestigateLastKnownLocation"),
	SpiralSearch    UMETA(DisplayName="SpiralSearch")
};

UENUM(BlueprintType)
enum class EDragonInstinct : uint8
{
	Roaming     UMETA(DisplayName="Roaming"),
	Stalking    UMETA(DisplayName="Stalking"),
	Threatening UMETA(DisplayName="Threatening"),
	Attacking   UMETA(DisplayName="Attacking"),
	Searching   UMETA(DisplayName="Searching"),
	ReturningHome UMETA(DisplayName="ReturningHome"),
	Resting     UMETA(DisplayName="Resting")
	
};