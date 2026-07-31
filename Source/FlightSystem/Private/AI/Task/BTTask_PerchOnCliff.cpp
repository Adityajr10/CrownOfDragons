// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Task/BTTask_PerchOnCliff.h"
#include "AIController.h"
#include "AI/Component/DragonAbilityComponent.h"
#include "AI/Ability/DragonAbility_PerchOnCliff.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_PerchOnCliff::UBTTask_PerchOnCliff()
{
	NodeName = "Perch On Cliff";
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_PerchOnCliff::ExecuteTask(
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

	FVector PerchLocation =
		OwnerComp.GetBlackboardComponent()
		->GetValueAsVector(PerchLocationKey.SelectedKeyName);

	UDragonAbility_PerchOnCliff* Ability =
		NewObject<UDragonAbility_PerchOnCliff>(Dragon, AbilityClass);

	Ability->SetPerchLocation(PerchLocation);

	AbilityComp->StartAbility(Ability, nullptr);

	return EBTNodeResult::InProgress;
}

void UBTTask_PerchOnCliff::TickTask(
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