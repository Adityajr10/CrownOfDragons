// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_FlyToAirTarget.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "DragonFlightInterface.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "AIController.h"
#include "DragonFlightComponent.h"

#include "DragonBaseAI.h"
#include "AI/Component/DragonAbilityComponent.h"
#include "AI/Ability/DragonAbility.h"

UBTTask_FlyToAirTarget::UBTTask_FlyToAirTarget()
{
	NodeName = "Fly To Air Target";
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_FlyToAirTarget::ExecuteTask(
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

	UDragonAbility* Ability =
		NewObject<UDragonAbility>(Dragon, AbilityClass);

	AbilityComp->StartAbility(Ability, nullptr);

	return EBTNodeResult::InProgress;
}

void UBTTask_FlyToAirTarget::TickTask(
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
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}