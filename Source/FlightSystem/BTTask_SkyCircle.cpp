// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_SkyCircle.h"
#include "DragonBaseAI.h"
#include "DragonFlightComponent.h"
#include "AIController.h"
#include "AI/Ability/DragonAbility_SkyCircle.h"
#include "AI/Component/DragonAbilityComponent.h"

UBTTask_SkyCircle::UBTTask_SkyCircle()
{
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_SkyCircle::ExecuteTask(
	UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
		return EBTNodeResult::Failed;

	ADragonBaseAI* Dragon =
		Cast<ADragonBaseAI>(AIController->GetPawn());

	if (!Dragon)
		return EBTNodeResult::Failed;

	UDragonAbilityComponent* AbilityComp =
		Dragon->FindComponentByClass<UDragonAbilityComponent>();

	if (!AbilityComp)
		return EBTNodeResult::Failed;

	UDragonAbility* Ability =
	NewObject<UDragonAbility>(Dragon, AbilityClass);

	AbilityComp->StartAbility(Ability, nullptr);

	return EBTNodeResult::InProgress;
}

void UBTTask_SkyCircle::TickTask(
	UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory,
	float DeltaSeconds)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	ADragonBaseAI* Dragon =
		Cast<ADragonBaseAI>(AIController->GetPawn());

	if (!Dragon)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	UDragonAbilityComponent* AbilityComp =
		Dragon->FindComponentByClass<UDragonAbilityComponent>();

	if (!AbilityComp)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	if (!AbilityComp->IsAbilityActive())
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}





/*
EBTNodeResult::Type UBTTask_SkyCircle::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return EBTNodeResult::InProgress;
}

void UBTTask_SkyCircle::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	ADragonBaseAI* Dragon = Cast<ADragonBaseAI>(OwnerComp.GetAIOwner()->GetPawn());
	if (!Dragon) { FinishLatentTask(OwnerComp, EBTNodeResult::Failed); return; }

	UDragonFlightComponent* Flight = Dragon->FindComponentByClass<UDragonFlightComponent>();
	if (!Flight) return;

	Dragon->CircleAngle += DeltaSeconds * 0.4f;

	FVector Center = Dragon->StartLocation;
	float Height = FMath::FRandRange(Dragon->CircleHeightMin, Dragon->CircleHeightMax);

	FVector Target;
	Target.X = Center.X + FMath::Cos(Dragon->CircleAngle) * Dragon->CircleRadius;
	Target.Y = Center.Y + FMath::Sin(Dragon->CircleAngle) * Dragon->CircleRadius;
	Target.Z = Center.Z + Height;

	Flight->SetAirTarget(Target);
}
*/
