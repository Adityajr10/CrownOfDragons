// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_GroundStrafe.generated.h"

class UDragonAbility;
/**
 * 
 */
UCLASS()
class FLIGHTSYSTEM_API UBTTask_GroundStrafe : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTTask_GroundStrafe();

	virtual EBTNodeResult::Type ExecuteTask(
		UBehaviorTreeComponent& OwnerComp,
		uint8* NodeMemory) override;

	virtual void TickTask(
		UBehaviorTreeComponent& OwnerComp,
		uint8* NodeMemory,
		float DeltaSeconds) override;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UDragonAbility> AbilityClass;
	virtual uint16 GetInstanceMemorySize() const override;
};