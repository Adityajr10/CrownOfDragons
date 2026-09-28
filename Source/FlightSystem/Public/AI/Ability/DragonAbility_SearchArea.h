#pragma once

#include "CoreMinimal.h"
#include "DragonAbility.h"
#include "DragonAbility_SearchArea.generated.h"

UCLASS()
class FLIGHTSYSTEM_API UDragonAbility_SearchArea : public UDragonAbility
{
	GENERATED_BODY()
public:
	void SetSearchLocation(const FVector& InLocation);

	virtual void Start(ADragonBaseAI* InOwner, AActor* InTarget) override;
	virtual void Tick(float DeltaTime) override;
	virtual bool IsFinished() const override;
	virtual bool CanBeInterrupted() const override;
	virtual void Abort(EDragonInterruptReason Reason) override;

protected:

	UPROPERTY(EditDefaultsOnly, Category="Search")
	float SearchRadius = 2000.f;

	UPROPERTY(EditDefaultsOnly, Category="Search")
	float SearchHeight = 2500.f;

	UPROPERTY(EditDefaultsOnly, Category="Search")
	float CircleSpeed = 0.5f;

	UPROPERTY(EditDefaultsOnly, Category="Search")
	float SearchDuration = 6.f;

	// How close dragon must get to search area before circling starts
	UPROPERTY(EditDefaultsOnly, Category="Search")
	float ArrivalThreshold = 600.f;

	FVector SearchLocation;

	bool  bReachedSearchArea = false;
	float CircleAngle        = 0.f;
	float AbilityTimer       = 0.f;
	bool  bFinished          = false;
};