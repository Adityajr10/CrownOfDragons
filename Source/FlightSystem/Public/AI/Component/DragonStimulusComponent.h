#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AI/Data/DragonAITypes.h"
#include "DragonStimulusComponent.generated.h"

class ADragonBaseAI;

USTRUCT(BlueprintType)
struct FDragonStimulusInfo
{
	GENERATED_BODY()

	UPROPERTY()
	EDragonStimulus Type = EDragonStimulus::None;

	UPROPERTY()
	float Priority = 0.f;

	UPROPERTY()
	AActor* Source = nullptr;

	UPROPERTY()
	float Value = 0.f;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDragonStimulus, const FDragonStimulusInfo&, Stimulus);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FLIGHTSYSTEM_API UDragonStimulusComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UDragonStimulusComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;

	// ---- Delegate ----
	UPROPERTY(BlueprintAssignable)
	FOnDragonStimulus OnStimulusReceived;

	// ---- API: call from damage handling ----

	UFUNCTION(BlueprintCallable, Category="Stimulus")
	void ReportDamage(float Amount, AActor* Instigator);

	// ---- Health tracking ----
	// Read from DragonBaseAI each tick
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Stimulus")
	float HealthPercent = 1.f;

	float PreviousHealthPercent = 1.f;

	// ---- Thresholds ----

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stimulus|Thresholds")
	float LowHealthThreshold = 0.4f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stimulus|Thresholds")
	float CriticalHealthThreshold = 0.15f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stimulus|Thresholds")
	float BurstDamageThreshold = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stimulus|Thresholds")
	float LowEnergyThreshold = 0.3f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stimulus|Thresholds")
	float CriticalEnergyThreshold = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stimulus|Thresholds")
	float EnergyRecoveredThreshold = 0.6f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stimulus|Cooldown")
	float StimulusCooldown = 1.0f;

protected:

	UPROPERTY()
	ADragonBaseAI* OwnerDragon = nullptr;

	TMap<EDragonStimulus, float> LastStimulusTime;

	bool bHealthLowFired = false;
	bool bHealthCriticalFired = false;
	bool bEnergyLowFired = false;
	bool bEnergyCriticalFired = false;
	bool bEnergyRecoveredFired = false;

	void FireStimulus(EDragonStimulus Type, float Priority, AActor* Source = nullptr, float Value = 0.f);
	void CheckEnergyThresholds(float EnergyPercent);
};