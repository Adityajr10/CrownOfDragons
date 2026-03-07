// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_FlyToAirTarget.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "DragonFlightInterface.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "AIController.h"
#include "DragonFlightComponent.h"

UBTTask_FlyToAirTarget::UBTTask_FlyToAirTarget()
{
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_FlyToAirTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return EBTNodeResult::InProgress;
}

void UBTTask_FlyToAirTarget::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	APawn* Pawn = OwnerComp.GetAIOwner()->GetPawn();
	if (!Pawn)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	UDragonFlightComponent* Flight = Pawn->FindComponentByClass<UDragonFlightComponent>();
	if (!Flight)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	if (Flight->IsAtTarget())
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}