// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraSystem.h"
#include "Components/ActorComponent.h"
#include "DragonFireBreathComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FLIGHTSYSTEM_API UDragonFireBreathComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UDragonFireBreathComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Fire")
	UNiagaraSystem* FireEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Fire")
	FName MouthSocket = "Tongue";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Fire")
	FName HeadBone = "head";

	USceneComponent* AimPivot;

	UNiagaraComponent* ActiveFire = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Fire")
	UAnimMontage* FireMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Fire")
	float FireInterval = 0.2f;

	bool bIsFiring = false;
	float FireCooldown = 5.f;
	bool bOnCooldown = false;
	bool bCanFire = true;

	FTimerHandle BreathTimer;
	FTimerHandle CooldownTimer;

	float FireDuration = 1.5f;
	FTimerHandle StopFireTimer;

	void StartFire(AActor* Target);
	void ResetCooldown();
	void StopFire();

protected:
	virtual void BeginPlay() override;

private:
	FTimerHandle FireTimer;
	AActor* CurrentTarget = nullptr;
	void EmitFire();
};