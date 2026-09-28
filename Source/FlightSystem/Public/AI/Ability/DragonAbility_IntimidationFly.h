#pragma once

#include "CoreMinimal.h"
#include "DragonAbility.h"
#include "DragonAbility_IntimidationFly.generated.h"

class UAnimMontage;

UCLASS()
class FLIGHTSYSTEM_API UDragonAbility_IntimidationFly : public UDragonAbility
{
	GENERATED_BODY()
public:

	virtual void Start(ADragonBaseAI* InOwner, AActor* InTarget) override;
	virtual void Tick(float DeltaTime) override;
	virtual bool IsFinished() const override;
	
	virtual bool CanBeInterrupted() const override;
	virtual void Abort(EDragonInterruptReason Reason) override;

protected:

	UPROPERTY(EditDefaultsOnly, Category="Intimidation")
	float CircleRadius = 2500.f;

	UPROPERTY(EditDefaultsOnly, Category="Intimidation")
	float CircleHeight = 3500.f;

	UPROPERTY(EditDefaultsOnly, Category="Intimidation")
	float CircleSpeed = 0.4f;

	// How many full circles before ability ends
	// Set to 0 to use IntimidationDuration instead
	UPROPERTY(EditDefaultsOnly, Category="Intimidation")
	int32 CirclesBeforeFinish = 2;

	// Fallback duration if CirclesBeforeFinish is 0
	UPROPERTY(EditDefaultsOnly, Category="Intimidation")
	float IntimidationDuration = 8.f;

	UPROPERTY(EditDefaultsOnly, Category="Intimidation")
	float MontageInterval = 2.5f;

	UPROPERTY(EditDefaultsOnly, Category="Intimidation")
	float ArrivalThreshold = 400.f;

	UPROPERTY(EditDefaultsOnly, Category="Intimidation")
	TArray<UAnimMontage*> IntimidationMontages;

	// Phase flags
	bool bReachedAbovePlayer = false;
	bool bFinished           = false;

	float CircleAngle        = 0.f;
	float TotalAngleTraveled = 0.f; // accumulated radians
	float AbilityTimer       = 0.f;
	float MontageTimer       = 0.f;
};