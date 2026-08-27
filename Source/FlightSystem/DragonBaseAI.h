#pragma once

#include "CoreMinimal.h"
#include "DragonFireBreathComponent.h"
#include "DragonFlightComponent.h"
#include "GameFramework/Character.h"
#include "DragonBaseAI.generated.h"

class UDragonAbilityComponent;
class UDragonStimulusComponent;

UCLASS()
class FLIGHTSYSTEM_API ADragonBaseAI : public ACharacter
{
	GENERATED_BODY()

public:
	ADragonBaseAI();

protected:
	virtual void BeginPlay() override;

public:
	void TakeoffFromPerch();
	void StartDiveBomb(AActor* Target);
	void OnDiveImpact();

	// ---- Components ----

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UDragonFlightComponent* FlightComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UDragonFireBreathComponent* FireBreath;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dragon|Ability")
	UDragonAbilityComponent* AbilityComponent;

	// NEW: Stimulus component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Dragon|AI")
	UDragonStimulusComponent* StimulusComponent;

	// ---- State ----

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsFlying;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Canplay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector StartLocation;

	// ---- Patrol ----

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Flight|Patrol")
	float CircleRadius = 12000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Flight|Patrol")
	float CircleHeightMin = 2000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Flight|Patrol")
	float CircleHeightMax = 3000.f;

	float CircleAngle = 0.f;

	// ---- Perch ----

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Perch")
	float PerchHeightOffset = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Perch")
	float PerchDuration = 5.f;

	bool bIsPerched = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Perch|Anim")
	UAnimMontage* WingFoldMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Perch|Anim")
	UAnimMontage* PerchRoarMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Perch|Anim")
	UAnimMontage* TakeoffMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Perch")
	float PerchWatchTime = 6.f;

	// ---- DiveBomb ----

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="DiveBomb")
	float DiveHeight = 9000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="DiveBomb")
	float DiveSpeed = 9000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="DiveBomb")
	UAnimMontage* DiveStartMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="DiveBomb")
	UAnimMontage* DiveEndMontage;

	AActor* DiveTarget = nullptr;
	bool bIsClimbing = false;
	bool bIsDiving = false;

	// ---- Strafe ----

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Strafe")
	float StrafeHeight = 400.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Strafe")
	float StrafeDistance = 12000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Strafe")
	float StrafeSpeed = 3500.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Strafe")
	UAnimMontage* StrafeMontage;

	// ---- Blueprint events ----

	UFUNCTION(BlueprintImplementableEvent)
	void EnableFlyingMode();

	UFUNCTION(BlueprintImplementableEvent)
	void DisableFlyingMode();

	// ---- Health ----

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Health")
	float MaxHealth = 100.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Health")
	float CurrentHealth = 100.f;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	float LastDamageTime = 0.f;

	UPROPERTY(EditAnywhere, Category="Health")
	float DamageCooldown = 1.0f;
	
	
	UFUNCTION(BlueprintCallable)
	void IncreaseHealth();

	UFUNCTION()
	void HealthModification();
	
	
	FTimerHandle HealthTimerHandle;
};