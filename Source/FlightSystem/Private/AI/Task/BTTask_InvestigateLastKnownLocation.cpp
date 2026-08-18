// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Task/BTTask_InvestigateLastKnownLocation.h"
#include "AIController.h"
#include "AI/Component/DragonAbilityComponent.h"
#include "AI/Ability/DragonAbility_InvestigateLastKnownLocation.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_InvestigateLastKnownLocation::UBTTask_InvestigateLastKnownLocation()
{
	NodeName = "Investigate Last Known Location";
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_InvestigateLastKnownLocation::ExecuteTask(
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

	FVector InvestigateLocation =
		OwnerComp.GetBlackboardComponent()
		->GetValueAsVector(
			InvestigateLocationKey.SelectedKeyName);

	UDragonAbility_InvestigateLastKnownLocation* Ability =
		NewObject<UDragonAbility_InvestigateLastKnownLocation>(
			Dragon,
			AbilityClass);

	Ability->SetInvestigateLocation(InvestigateLocation);

	AbilityComp->StartAbility(Ability, nullptr);

	return EBTNodeResult::InProgress;
}

void UBTTask_InvestigateLastKnownLocation::TickTask(
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