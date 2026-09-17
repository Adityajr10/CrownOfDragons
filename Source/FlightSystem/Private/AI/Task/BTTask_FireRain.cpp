// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Task/BTTask_FireRain.h"
#include "AIController.h"
#include "AI/Component/DragonAbilityComponent.h"
#include "AI/Ability/DragonAbility.h"
#include "Kismet/GameplayStatics.h"

UBTTask_FireRain::UBTTask_FireRain()
{
	NodeName = "Fire Rain";
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_FireRain::ExecuteTask(
	UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory)
{
	AAIController* AI = OwnerComp.GetAIOwner();
	if (!AI) return EBTNodeResult::Failed;

	ADragonBaseAI* Dragon = Cast<ADragonBaseAI>(AI->GetPawn());
	if (!Dragon) return EBTNodeResult::Failed;

	UDragonAbilityComponent* AbilityComp =
		Dragon->FindComponentByClass<UDragonAbilityComponent>();
	Dragon->EnableFlyingMode();

	if (!AbilityComp || !AbilityClass)
		return EBTNodeResult::Failed;

	AActor* Player = UGameplayStatics::GetPlayerPawn(Dragon, 0);

	UDragonAbility* Ability =
		NewObject<UDragonAbility>(Dragon, AbilityClass);

	AbilityComp->StartAbility(Ability, Player);

	return EBTNodeResult::InProgress;
}

void UBTTask_FireRain::TickTask(
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