// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_AirStrafe.generated.h"


class UDragonAbility;
UCLASS()
class FLIGHTSYSTEM_API UBTTask_AirStrafe : public UBTTaskNode
{
	GENERATED_BODY()
public:

	UBTTask_AirStrafe();

	virtual EBTNodeResult::Type ExecuteTask(
		UBehaviorTreeComponent& OwnerComp,
		uint8* NodeMemory) override;

	virtual void TickTask(
		UBehaviorTreeComponent& OwnerComp,
		uint8* NodeMemory,
		float DeltaSeconds) override;

protected:

	/* Ability to execute */
	UPROPERTY(EditAnywhere, Category="Ability")
	TSubclassOf<UDragonAbility> AbilityClass;

	/* Location where dragon will perform strafe */
	UPROPERTY(EditAnywhere, Category="AI")
	FBlackboardKeySelector StrafeLocationKey;
};