#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Components/ActorComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "AI/Data/DragonAITypes.h"
#include "AI/Data/DragonAbilityData.h"
#include "AI/Data/DragonPersonalitySettings.h"
#include "AI/Component/DragonStimulusComponent.h"
#include "DragonAIBehaviourComponent.generated.h"

class UDragonAbilityComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FLIGHTSYSTEM_API UDragonAIBehaviourComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UDragonAIBehaviourComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(
		float DeltaTime, ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;

	// ---- Existing: Core references ----

	UPROPERTY()
	AAIController* OwnerAIController;
	UPROPERTY()
	UBlackboardComponent* BlackboardComponent;
	UPROPERTY()
	AActor* TargetActor = nullptr;

	// ---- Personality ----

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	FDragonPersonalitySettings PersonalitySettings;

	// Empty = every instinct allowed. Otherwise selection is limited to
	// the listed instincts (ReturningHome territory override still applies)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	TArray<EDragonInstinct> AllowedInstincts;

	UFUNCTION(BlueprintCallable)
	bool IsInstinctAllowed(EDragonInstinct Instinct) const;

	// ---- Ability data ----

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	TArray<FDragonAbilityData> AbilitySettings;

	// ---- Current state ----

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI")
	EDragonState CurrentState = EDragonState::Idle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI")
	EDragonAbilityType CurrentAbility = EDragonAbilityType::None;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI")
	EDragonInstinct CurrentInstinct = EDragonInstinct::Roaming;

	// ---- Energy ----

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Energy")
	float MaxEnergy = 100.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Energy")
	float CurrentEnergy = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Energy")
	float EnergyRegenRate = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Energy")
	float RestingRegenMultiplier = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Energy")
	float RestingExitThreshold = 0.7f;

	void ConsumeEnergy(float Amount);

	// ---- Cooldown ----

	UPROPERTY()
	TMap<EDragonAbilityType, float> LastAbilityUseTime;

	bool CanUseAbility(const FDragonAbilityData& AbilityData) const;
	void UseAbility(const FDragonAbilityData& AbilityData);

	// ---- Brain ----

	UFUNCTION(BlueprintCallable)
	void EvaluateSituation();
	UFUNCTION(BlueprintCallable)
	void SelectInstinct();
	UFUNCTION(BlueprintCallable)
	void SelectAbility();
	float GetDistanceToTarget() const;
	UFUNCTION(BlueprintCallable)
	void UpdateBlackboard();

	// ---- Ability memory ----

	UPROPERTY()
	EDragonAbilityType LastUsedAbility = EDragonAbilityType::None;
	UPROPERTY()
	EDragonAbilityType SecondLastUsedAbility = EDragonAbilityType::None;

	// ---- Combat timing ----

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat")
	float MinAttackInterval = 2.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat")
	float MaxAttackInterval = 4.0f;
	UPROPERTY()
	float NextAttackTime = 0.f;
	bool CanPerformAttack() const;

	// ---- Instinct persistence ----

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	float MinInstinctDuration = 2.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	float MaxInstinctDuration = 5.0f;
	float NextInstinctChangeTime = 0.f;

	// ---- Target ----

	void SetTargetActor(AActor* NewTarget);

	UPROPERTY()
	FVector LastKnownTargetLocation;
	UPROPERTY(EditAnywhere, Category="AI")
	float TargetMemoryDuration = 10.f;
	float LastSeenTime = 0.f;

	bool IsValidTarget(AActor* Actor) const;
	float GetTargetAltitudeDifference() const;
	FVector PredictTargetLocation(float PredictionTime) const;

	// ---- Territory ----

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Territory")
	FVector HomeLocation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Territory")
	float TerritoryRadius = 30000.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Territory")
	float MaxChaseDistance = 40000.f;
	float GetDistanceFromHome() const;
	bool IsInsideTerritory() const;

	// ---- Ability selection ----

	float NextAbilityDecisionTime = 0.f;
	float AbilityDecisionInterval = 2.0f;

	bool bAbilityRunning = false;
	void OnAbilityStarted();
	void OnAbilityFinished();
	void SetBlackboard(UBlackboardComponent* InBlackboard);

	UPROPERTY(EditAnywhere, Category="AI")
	float AbilityRetryInterval = 1.5f;

	// Re-select if the BT never picks up the chosen ability (missing tree node)
	UPROPERTY(EditAnywhere, Category="AI")
	float AbilityStallTimeout = 3.0f;
	float AbilityStallTime = 0.f;

	FTimerHandle AbilityRetryHandle;
	int32 AbilityRetryCount = 0;
	UPROPERTY(EditAnywhere, Category="AI")
	int32 MaxAbilityRetries = 3;

	const FDragonAbilityData* FindAbilityData(EDragonAbilityType InAbilityType) const;

	// ============================================================
	// NEW: Stimulus handling
	// ============================================================

	UFUNCTION()
	void OnStimulusReceived(const FDragonStimulusInfo& Stimulus);

	// ============================================================
	// NEW: Energy zones
	// ============================================================

	EDragonEnergyZone GetEnergyZone() const;
	float GetEnergyScoreModifier(const FDragonAbilityData& AbilityData) const;

	// ============================================================
	// NEW: Reaction phase
	// ============================================================

	bool bInReactionPhase = false;
	EDragonInterruptReason LastInterruptReason = EDragonInterruptReason::None;
	void SelectReactionAbility();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI|Reaction")
	float ReactionRepositionDistance = 2000.f;

	// ============================================================
	// NEW: Health-based personality thresholds
	// ============================================================

	// Smooth danger curve: 0 at 60%+ HP, 1.0 at 0% HP
	float GetDangerLevel() const;
	// Effective danger after courage reduces it
	float GetEffectiveDanger() const;

	// ============================================================
	// NEW: Overhead debug display
	// ============================================================

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Debug")
	bool bShowDebugInfo = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Debug")
	float DebugTextHeightOffset = 350.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Debug")
	float DebugTextScale = 1.2f;

	void DrawOverheadDebug() const;

protected:

	bool bReactingStimulusThisFrame = false;

	FDragonStimulusInfo PendingStimulus;
	bool bHasPendingStimulus = false;
};