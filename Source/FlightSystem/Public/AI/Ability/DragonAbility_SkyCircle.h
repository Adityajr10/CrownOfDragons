#pragma once

#include "CoreMinimal.h"
#include "AI/Ability/DragonAbility.h"
#include "DragonAbility_SkyCircle.generated.h"

UCLASS(Blueprintable)
class FLIGHTSYSTEM_API UDragonAbility_SkyCircle : public UDragonAbility
{
	GENERATED_BODY()
public:

	virtual void Start(ADragonBaseAI* InOwner, AActor* InTarget) override;
	virtual void Tick(float DeltaTime) override;
	virtual bool IsFinished() const override;
	
	virtual bool CanBeInterrupted() const override;
	virtual void Abort(EDragonInterruptReason Reason) override;

protected:

	UPROPERTY(EditDefaultsOnly, Category="SkyCircle")
	float CircleDuration = 8.f;
	UPROPERTY(EditDefaultsOnly, Category="SkyCircle")
	float CircleSpeed = 0.9f;
	float CircleAngle  = 0.f;
	float AbilityTimer = 0.f;
	bool  bFinished    = false;
};