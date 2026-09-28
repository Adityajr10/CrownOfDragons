// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/Ability/DragonAbility.h"
#include "DragonAbility_SunDive.generated.h"

class ADragonShockwaveActor;
class USoundBase;

/**
 * Boss move: climb until vanishing overhead, silently shadow the player
 * from extreme altitude, then dive straight down at extreme speed.
 */
UCLASS()
class FLIGHTSYSTEM_API UDragonAbility_SunDive : public UDragonAbility
{
	GENERATED_BODY()

public:

	virtual void Start(ADragonBaseAI* InOwner, AActor* InTarget) override;
	virtual void Tick(float DeltaTime) override;
	virtual bool IsFinished() const override;
	virtual bool CanBeInterrupted() const override;
	virtual void Abort(EDragonInterruptReason Reason) override;

protected:

	enum class ESunDivePhase : uint8
	{
		Climb,
		Vanish,
		Dive
	};

	ESunDivePhase Phase = ESunDivePhase::Climb;

	UPROPERTY(EditDefaultsOnly, Category="SunDive")
	float VanishHeight = 12000.f;

	UPROPERTY(EditDefaultsOnly, Category="SunDive")
	float ClimbSpeed = 6000.f;

	UPROPERTY(EditDefaultsOnly, Category="SunDive")
	float VanishTimeMin = 2.f;

	UPROPERTY(EditDefaultsOnly, Category="SunDive")
	float VanishTimeMax = 4.f;

	UPROPERTY(EditDefaultsOnly, Category="SunDive")
	float ShadowSpeed = 2000.f;

	UPROPERTY(EditDefaultsOnly, Category="SunDive")
	float DiveSpeed = 16000.f;

	UPROPERTY(EditDefaultsOnly, Category="SunDive")
	bool bHideDuringVanish = true;

	UPROPERTY(EditDefaultsOnly, Category="SunDive")
	USoundBase* WingbeatSound;

	UPROPERTY(EditDefaultsOnly, Category="SunDive")
	USoundBase* DiveSound;

	UPROPERTY(EditDefaultsOnly, Category="SunDive")
	UAnimMontage* ImpactMontage;

	UPROPERTY(EditDefaultsOnly, Category="SunDive")
	TSubclassOf<ADragonShockwaveActor> ShockwaveClass;

	FVector DiveTarget;
	float VanishTime = 3.f;
	float PhaseTimer = 0.f;

	void SetDragonHidden(bool bHidden);
};
