// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_SpiralSearch.generated.h"

class UDragonAbility;
/**
 * 
 */
UCLASS()
class FLIGHTSYSTEM_API UBTTask_SpiralSearch : public UBTTaskNode
{
	GENERATED_BODY()
	
public:

	UBTTask_SpiralSearch();

	virtual EBTNodeResult::Type ExecuteTask(
		UBehaviorTreeComponent& OwnerComp,
		uint8* NodeMemory) override;

	virtual void TickTask(
		UBehaviorTreeComponent& OwnerComp,
		uint8* NodeMemory,
		float DeltaSeconds) override;

protected:

	UPROPERTY(EditAnywhere, Category="Ability")
	TSubclassOf<UDragonAbility> AbilityClass;

	UPROPERTY(EditAnywhere, Category="AI")
	FBlackboardKeySelector SearchLocationKey;
};