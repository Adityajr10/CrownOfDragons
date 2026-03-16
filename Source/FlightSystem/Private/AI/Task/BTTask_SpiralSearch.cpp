// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Task/BTTask_SpiralSearch.h"
#include "AIController.h"
#include "AI/Component/DragonAbilityComponent.h"
#include "AI/Ability/DragonAbility_SpiralSearch.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_SpiralSearch::UBTTask_SpiralSearch()
{
	NodeName = "Spiral Search";
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_SpiralSearch::ExecuteTask(
	UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory)
{
	AAIController* AI = OwnerComp.GetAIOwner();
	if (!AI) return EBTNodeResult::Failed;

	ADragonBaseAI* Dragon =
		Cast<ADragonBaseAI>(AI->GetPawn());

	if (!Dragon) return EBTNodeResult::Failed;

	UDragonAbilityComponent* AbilityComp =
		Dragon->FindComponentByClass<UDragonAbilityComponent>();

	if (!AbilityComp || !AbilityClass)
		return EBTNodeResult::Failed;

	FVector SearchLocation =
		OwnerComp.GetBlackboardComponent()
		->GetValueAsVector(
			SearchLocationKey.SelectedKeyName);

	UDragonAbility_SpiralSearch* Ability =
		NewObject<UDragonAbility_SpiralSearch>(
			Dragon,
			AbilityClass);

	Ability->SetSearchLocation(SearchLocation);

	AbilityComp->StartAbility(Ability, nullptr);

	return EBTNodeResult::InProgress;
}

void UBTTask_SpiralSearch::TickTask(
	UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory,
	float DeltaSeconds)
{
	AAIController* AI = OwnerComp.GetAIOwner();
	if (!AI) return;

	ADragonBaseAI* Dragon =
		Cast<ADragonBaseAI>(AI->GetPawn());

	if (!Dragon) return;

	UDragonAbilityComponent* AbilityComp =
		Dragon->FindComponentByClass<UDragonAbilityComponent>();

	if (!AbilityComp->IsAbilityActive())
	{
		FinishLatentTask(
			OwnerComp,
			EBTNodeResult::Succeeded);
	}
}