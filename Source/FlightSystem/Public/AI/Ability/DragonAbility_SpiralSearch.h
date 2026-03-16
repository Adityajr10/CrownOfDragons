#pragma once

#include "CoreMinimal.h"
#include "DragonAbility.h"
#include "DragonAbility_SpiralSearch.generated.h"

UCLASS()
class FLIGHTSYSTEM_API UDragonAbility_SpiralSearch : public UDragonAbility
{
	GENERATED_BODY()
public:

	void SetSearchLocation(const FVector& InLocation);

	virtual void Start(ADragonBaseAI* InOwner, AActor* InTarget) override;
	virtual void Tick(float DeltaTime) override;
	virtual bool IsFinished() const override;

protected:

	UPROPERTY(EditDefaultsOnly, Category="Search")
	float InitialRadius = 500.f;

	UPROPERTY(EditDefaultsOnly, Category="Search")
	float RadiusGrowth = 150.f; 

	UPROPERTY(EditDefaultsOnly, Category="Search")
	float SearchHeight = 2500.f;

	UPROPERTY(EditDefaultsOnly, Category="Search")
	float SpiralSpeed =  0.3f;

	UPROPERTY(EditDefaultsOnly, Category="Search")
	float SearchDuration = 8.0f;

	// How close dragon must get before spiral starts
	UPROPERTY(EditDefaultsOnly, Category="Search")
	float ArrivalThreshold = 600.f;

	FVector SearchLocation;

	bool  bReachedSearchArea = false;
	bool  bFinished          = false;
	float CurrentRadius      = 0.f;
	float Angle              = 0.f;
	float AbilityTimer       = 0.f;
	// How close dragon must be to current spiral point before advancing
	UPROPERTY(EditDefaultsOnly, Category="Search")
	float SpiralStepThreshold = 300.f;
};