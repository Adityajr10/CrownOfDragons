// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_PlayerOrbitStrafe.generated.h"

/**
 * 
 */
UCLASS()
class FLIGHTSYSTEM_API UBTTask_PlayerOrbitStrafe : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTTask_PlayerOrbitStrafe();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	bool bFireStarted = false;

protected:
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;
};