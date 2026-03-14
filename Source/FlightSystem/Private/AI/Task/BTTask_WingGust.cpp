// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Task/BTTask_WingGust.h"
#include "AIController.h"
#include "AI/Ability/DragonAbility_WingGust.h"
#include "AI/Component/DragonAbilityComponent.h"
#include "AI/Ability/DragonAbility_WingGust.h"

UBTTask_WingGust::UBTTask_WingGust()
{
	NodeName = "Wing Gust";
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_WingGust::ExecuteTask(
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

	UDragonAbility_WingGust* Ability =
		NewObject<UDragonAbility_WingGust>(Dragon, AbilityClass);

	if (!Ability) return EBTNodeResult::Failed;

	Ability->SetGustMode(GustMode);

	AbilityComp->StartAbility(Ability, nullptr);

	return EBTNodeResult::InProgress;
}

void UBTTask_WingGust::TickTask(
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