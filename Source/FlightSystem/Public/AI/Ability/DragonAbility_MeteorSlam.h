// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/Ability/DragonAbility.h"
#include "DragonAbility_MeteorSlam.generated.h"

class ADragonShockwaveActor;

/**
 * Climb above the player, hang as a telegraph, crash down with a shockwave,
 * then stay grounded for a short vulnerable recovery.
 */
UCLASS()
class FLIGHTSYSTEM_API UDragonAbility_MeteorSlam : public UDragonAbility
{
	GENERATED_BODY()

public:

	virtual void Start(ADragonBaseAI* InOwner, AActor* InTarget) override;
	virtual void Tick(float DeltaTime) override;
	virtual bool IsFinished() const override;
	virtual bool CanBeInterrupted() const override;
	virtual void Abort(EDragonInterruptReason Reason) override;

protected:

	enum class ESlamPhase : uint8
	{
		Climb,
		Hang,
		Slam,
		Grounded
	};

	ESlamPhase Phase = ESlamPhase::Climb;

	UPROPERTY(EditDefaultsOnly, Category="MeteorSlam")
	float ClimbHeight = 3000.f;

	UPROPERTY(EditDefaultsOnly, Category="MeteorSlam")
	float ClimbSpeed = 4000.f;

	UPROPERTY(EditDefaultsOnly, Category="MeteorSlam")
	float HangTime = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category="MeteorSlam")
	float SlamSpeed = 12000.f;

	UPROPERTY(EditDefaultsOnly, Category="MeteorSlam")
	float GroundedTime = 2.0f;

	UPROPERTY(EditDefaultsOnly, Category="MeteorSlam")
	UAnimMontage* TelegraphMontage;

	UPROPERTY(EditDefaultsOnly, Category="MeteorSlam")
	UAnimMontage* ImpactMontage;

	UPROPERTY(EditDefaultsOnly, Category="MeteorSlam")
	TSubclassOf<ADragonShockwaveActor> ShockwaveClass;

	FVector SlamTarget;
	float PhaseTimer = 0.f;

	void DoImpact();
	void ExitGrounded();
};
