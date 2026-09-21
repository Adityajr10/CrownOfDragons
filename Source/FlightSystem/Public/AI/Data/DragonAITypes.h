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
	Strafe      UMETA(DisplayName="Strafe"),      // NEW
	Rest        UMETA(DisplayName="Rest"),
	Flee        UMETA(DisplayName="Flee"),
	Perch       UMETA(DisplayName="Perch")
};

UENUM(BlueprintType)
enum class EDragonAbilityType : uint8
{
	None            UMETA(DisplayName="None"),

	// ATTACK ABILITIES
	DiveBomb        UMETA(DisplayName="DiveBomb"),
	FireBreath      UMETA(DisplayName="FireBreath"),
	FireRain        UMETA(DisplayName="FireRain"),
	ClawAttack      UMETA(DisplayName="ClawAttack"),
	WingGust        UMETA(DisplayName="WingGust"),

	// STRAFING (moved out of attacking)
	AirStrafe       UMETA(DisplayName="AirStrafe"),
	GroundStrafe    UMETA(DisplayName="GroundStrafe"),

	// THREATENING
	Roar            UMETA(DisplayName="Roar"),
	LowFlyThreat    UMETA(DisplayName="LowFlyThreat"),
	IntimidationFly UMETA(DisplayName="IntimidationFly"),

	// STALKING
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
	SpiralSearch    UMETA(DisplayName="SpiralSearch"),

	// SIGNATURE (appended at end — do not reorder, serialized by value)
	MeteorSlam      UMETA(DisplayName="MeteorSlam"),
	CarpetBomb      UMETA(DisplayName="CarpetBomb"),
	Cyclone         UMETA(DisplayName="Cyclone"),
	ConcussiveRoar  UMETA(DisplayName="ConcussiveRoar"),
	SunDive         UMETA(DisplayName="SunDive")
};

UENUM(BlueprintType)
enum class EDragonInstinct : uint8
{
	Roaming       UMETA(DisplayName="Roaming"),
	Stalking      UMETA(DisplayName="Stalking"),
	Threatening   UMETA(DisplayName="Threatening"),
	Strafing      UMETA(DisplayName="Strafing"),     // NEW
	Attacking     UMETA(DisplayName="Attacking"),
	Searching     UMETA(DisplayName="Searching"),
	ReturningHome UMETA(DisplayName="ReturningHome"),
	Resting       UMETA(DisplayName="Resting")
};

// ============================================================
// NEW: Stimulus system
// ============================================================

UENUM(BlueprintType)
enum class EDragonStimulus : uint8
{
	None               UMETA(DisplayName="None"),

	// Damage events
	DamageTaken        UMETA(DisplayName="DamageTaken"),
	HeavyDamageTaken   UMETA(DisplayName="HeavyDamageTaken"),
	HealthLow          UMETA(DisplayName="HealthLow"),
	HealthCritical     UMETA(DisplayName="HealthCritical"),

	// Energy events
	EnergyLow          UMETA(DisplayName="EnergyLow"),
	EnergyCritical     UMETA(DisplayName="EnergyCritical"),
	EnergyRecovered    UMETA(DisplayName="EnergyRecovered"),

	// NOTE: Target detection (acquired/lost) is handled by the AIController
	// via SetTargetActor(). The StimulusComponent does NOT duplicate that.
};

UENUM(BlueprintType)
enum class EDragonInterruptReason : uint8
{
	None           UMETA(DisplayName="None"),
	DamageFlinch   UMETA(DisplayName="DamageFlinch"),
	HealthFlee     UMETA(DisplayName="HealthFlee"),
	EnergyDepleted UMETA(DisplayName="EnergyDepleted"),
	Timeout        UMETA(DisplayName="Timeout"),
	OutOfRange     UMETA(DisplayName="OutOfRange"),
	Forced         UMETA(DisplayName="Forced"),
};

UENUM(BlueprintType)
enum class EDragonAbilityPriority : uint8
{
	Normal    UMETA(DisplayName="Normal"),
	Reactive  UMETA(DisplayName="Reactive"),
	Desperate UMETA(DisplayName="Desperate"),
	Efficient UMETA(DisplayName="Efficient"),
};

UENUM(BlueprintType)
enum class EDragonEnergyZone : uint8
{
	Critical UMETA(DisplayName="Critical"),   // 0-20%
	Low      UMETA(DisplayName="Low"),        // 20-50%
	Normal   UMETA(DisplayName="Normal"),     // 50-75%
	Full     UMETA(DisplayName="Full"),       // 75-100%
};