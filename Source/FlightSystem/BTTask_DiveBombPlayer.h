// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_DiveBombPlayer.generated.h"

/**
 * 
 */
UCLASS()
class FLIGHTSYSTEM_API UBTTask_DiveBombPlayer : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTTask_DiveBombPlayer();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual uint16 GetInstanceMemorySize() const override;
};