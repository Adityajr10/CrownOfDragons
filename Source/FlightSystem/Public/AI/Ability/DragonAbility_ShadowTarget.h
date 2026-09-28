#pragma once

#include "CoreMinimal.h"
#include "DragonAbility.h"
#include "DragonAbility_ShadowTarget.generated.h"

UCLASS()
class FLIGHTSYSTEM_API UDragonAbility_ShadowTarget : public UDragonAbility
{
	GENERATED_BODY()

public:

	virtual void Start(ADragonBaseAI* InOwner, AActor* InTarget) override;
	virtual void Tick(float DeltaTime) override;
	virtual bool IsFinished() const override;

protected:

	UPROPERTY(EditDefaultsOnly, Category="Shadow")
	float ShadowHeight = 3500.f;

	// How long the dragon follows the player once it arrives above them
	UPROPERTY(EditDefaultsOnly, Category="Shadow")
	float FollowDuration = 5.f;

	UPROPERTY(EditDefaultsOnly, Category="Shadow")
	float RotationSpeed = 4.f;

	// How close dragon must get above player before Phase 2 starts
	UPROPERTY(EditDefaultsOnly, Category="Shadow")
	float ArrivalThreshold = 1200.f;

	// Phase flags
	bool bReachedAbovePlayer = false;
	bool bFinished           = false;

	float AbilityTimer = 0.f; // only ticks after Phase 1 is done
};