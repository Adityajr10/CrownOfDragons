// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DragonAbility.h"
#include "DragonAbility_Roar.generated.h"


class UAnimMontage;

UCLASS()
class FLIGHTSYSTEM_API UDragonAbility_Roar : public UDragonAbility
{
	GENERATED_BODY()
	virtual void Start(ADragonBaseAI* InOwner, AActor* InTarget) override;
	virtual void Tick(float DeltaTime) override;

	virtual bool IsFinished() const override;

protected:

	UPROPERTY(EditDefaultsOnly, Category="Roar")
	TArray<UAnimMontage*> RoarMontages;

	UPROPERTY(EditDefaultsOnly, Category="Roar")
	float RotationSpeed = 4.f;

	bool bRoarStarted = false;

	UAnimMontage* CurrentMontage = nullptr;
};
