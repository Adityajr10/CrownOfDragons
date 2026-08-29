// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Task/BTTask_LowFlyThreat.h"
#include "AIController.h"
#include "AI/Component/DragonAbilityComponent.h"
#include "AI/Ability/DragonAbility.h"
#include "Kismet/GameplayStatics.h"

UBTTask_LowFlyThreat::UBTTask_LowFlyThreat()
{
	NodeName = "Low Fly Threat";
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_LowFlyThreat::ExecuteTask(
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

	AActor* Player =
		UGameplayStatics::GetPlayerPawn(Dragon, 0);

	UDragonAbility* Ability =
		NewObject<UDragonAbility>(Dragon, AbilityClass);

	AbilityComp->StartAbility(Ability, Player);

	return EBTNodeResult::InProgress;
}

void UBTTask_LowFlyThreat::TickTask(
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
			EBTNodeResult::Succeeded
		);
	}
}