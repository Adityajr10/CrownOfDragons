// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Task/BTTask_AirStrafe.h"
#include "AIController.h"
#include "AI/Ability/DragonAbility.h"
#include "AI/Component/DragonAbilityComponent.h"
#include "AI/Ability/DragonAbility_AirStrafe.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "FlightSystem/DragonBaseAI.h"

UBTTask_AirStrafe::UBTTask_AirStrafe()
{
	NodeName = "Air Strafe";
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_AirStrafe::ExecuteTask(
	UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory)
{
	AAIController* AI = OwnerComp.GetAIOwner();
	if (!AI) return EBTNodeResult::Failed;

	ADragonBaseAI* Dragon = Cast<ADragonBaseAI>(AI->GetPawn());
	if (!Dragon) return EBTNodeResult::Failed;

	UDragonAbilityComponent* AbilityComp =
		Dragon->FindComponentByClass<UDragonAbilityComponent>();

	if (!AbilityComp || !AbilityClass)
		return EBTNodeResult::Failed;

	/* Fetch strafe location from blackboard */
	FVector StrafeLocation =
		OwnerComp.GetBlackboardComponent()->GetValueAsVector(
			StrafeLocationKey.SelectedKeyName);

	/* Create ability */
	UDragonAbility_AirStrafe* Ability =
		NewObject<UDragonAbility_AirStrafe>(Dragon, AbilityClass);

	if (!Ability) return EBTNodeResult::Failed;

	/* Pass the location to ability */
	Ability->SetStrafeLocation(StrafeLocation);

	AbilityComp->StartAbility(Ability, nullptr);

	return EBTNodeResult::InProgress;
}

void UBTTask_AirStrafe::TickTask(
	UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory,
	float DeltaSeconds)
{
	AAIController* AI = OwnerComp.GetAIOwner();
	if (!AI) return;

	ADragonBaseAI* Dragon = Cast<ADragonBaseAI>(AI->GetPawn());
	if (!Dragon) return;

	UDragonAbilityComponent* AbilityComp =
		Dragon->FindComponentByClass<UDragonAbilityComponent>();

	if (!AbilityComp->IsAbilityActive())
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}