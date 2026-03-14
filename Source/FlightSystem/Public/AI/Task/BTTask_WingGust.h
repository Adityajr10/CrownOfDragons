// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_WingGust.generated.h"

class UDragonAbility;

UENUM(BlueprintType)
enum class EWingGustMode : uint8
{
	Ground,
	Air
};
UCLASS()
class FLIGHTSYSTEM_API UBTTask_WingGust : public UBTTaskNode
{
	GENERATED_BODY()
public:

	UBTTask_WingGust();

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

	/* Select if gust should happen on ground or air */
	UPROPERTY(EditAnywhere, Category="WingGust")
	EWingGustMode GustMode = EWingGustMode::Ground;
};