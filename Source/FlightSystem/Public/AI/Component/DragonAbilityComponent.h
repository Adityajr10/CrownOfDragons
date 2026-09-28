#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AI/Data/DragonAITypes.h"
#include "AI/Component/DragonStimulusComponent.h"
#include "DragonAbilityComponent.generated.h"

class UDragonAbility;
class ADragonBaseAI;
class UDragonAIBehaviourComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FLIGHTSYSTEM_API UDragonAbilityComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UDragonAbilityComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent(
		float DeltaTime, ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;

	void StartAbility(UDragonAbility* Ability, AActor* Target);
	bool IsAbilityActive() const;
	EDragonAbilityType GetActiveAbilityType() const;

	// ---- NEW: Interrupt API ----

	UFUNCTION(BlueprintCallable)
	bool RequestInterrupt(EDragonInterruptReason Reason);

	UFUNCTION(BlueprintCallable)
	void ForceInterrupt();

protected:

	UPROPERTY()
	ADragonBaseAI* OwnerDragon = nullptr;

	UPROPERTY()
	UDragonAbility* ActiveAbility = nullptr;

	UPROPERTY(EditAnywhere, Category="Ability")
	float MaxAbilityDuration = 7.0f;

	UPROPERTY(EditAnywhere, Category="Ability")
	float HardAbilityDuration = 14.0f;

	float AbilityRunningTime = 0.f;

	UPROPERTY()
	float CachedAbortDistanceThresholdSq = 0.f;

	// NEW: cached interrupt data from FDragonAbilityData
	bool bCachedCanBeInterrupted = true;
	float CachedForceInterruptHealthPercent = 0.f;

	// NEW: stimulus handler
	UFUNCTION()
	void OnStimulusWhileAbilityActive(const FDragonStimulusInfo& Stimulus);

	void FinishAbility(EDragonInterruptReason Reason = EDragonInterruptReason::None);

	UDragonAIBehaviourComponent* GetBehaviour() const;
	mutable UDragonAIBehaviourComponent* CachedBehaviour = nullptr;
	
	EDragonInterruptReason LastFinishReason = EDragonInterruptReason::None;
};