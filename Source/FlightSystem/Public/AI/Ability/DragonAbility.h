#pragma once

#include "CoreMinimal.h"
#include "AI/Data/DragonAITypes.h"
#include "UObject/NoExportTypes.h"
#include "FlightSystem/DragonBaseAI.h"
#include "DragonAbility.generated.h"

UCLASS(Blueprintable, BlueprintType)
class FLIGHTSYSTEM_API UDragonAbility : public UObject
{
	GENERATED_BODY()

public:

	/* Called when ability starts */
	virtual void Start(ADragonBaseAI* InOwner, AActor* InTarget);

	/* Called every tick while ability is active */
	virtual void Tick(float DeltaTime);

	/* Returns true when ability is finished */
	virtual bool IsFinished() const;

	/* Optional hook (for damage / hit events on OTHER actors) */
	virtual void OnAbilityHit(AActor* OtherActor, UPrimitiveComponent* OtherComp) {}

	// ============================================================
	// NEW: Interrupt system
	// ============================================================

	/**
	 * Can this ability be interrupted RIGHT NOW?
	 * Base returns true. Override in subclasses for phase logic.
	 * Example: DiveBomb returns false during dive phase.
	 */
	virtual bool CanBeInterrupted() const;

	/**
	 * Clean shutdown. Override to stop montages, destroy spawned actors.
	 * Always call Super::Abort(Reason) at end of override.
	 */
	virtual void Abort(EDragonInterruptReason Reason);

	/**
	 * Called when owner dragon takes damage while this ability runs.
	 * Use for flinch animations. Does NOT auto-abort.
	 */
	virtual void OnOwnerDamaged(float DamageAmount, float DamagePercent, AActor* Instigator) {}

protected:

	UPROPERTY()
	ADragonBaseAI* OwnerDragon = nullptr;

	UPROPERTY()
	AActor* TargetActor = nullptr;

	bool bFinished = false;
	bool bAborted = false;

public:

	int32 SpawnedCount = 0;
	int32 MaxSpawns = 3;

	UPROPERTY()
	EDragonAbilityType AbilityType = EDragonAbilityType::None;

	bool WasAborted() const { return bAborted; }
};