// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_DragonFireProjectile.generated.h"


class UDragonAbility;
UCLASS()
class FLIGHTSYSTEM_API UBTTask_DragonFireProjectile : public UBTTaskNode
{
	GENERATED_BODY()
public:

	UBTTask_DragonFireProjectile();

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
};