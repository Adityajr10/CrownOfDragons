// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_SetRandomAirTarget.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "DragonFlightComponent.h"
#include "DragonBaseAI.h"

EBTNodeResult::Type UBTTask_SetRandomAirTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return EBTNodeResult::Failed;  // ← ADD THIS
	
	APawn* Pawn = OwnerComp.GetAIOwner()->GetPawn();
	if (!Pawn) return EBTNodeResult::Failed;

	ADragonBaseAI* Dragon = Cast<ADragonBaseAI>(Pawn);
	if (!Dragon) return EBTNodeResult::Failed;

	UDragonFlightComponent* Flight = Dragon->FindComponentByClass<UDragonFlightComponent>();
	if (!Flight) return EBTNodeResult::Failed;

	const FVector Start = Dragon->StartLocation;

	FVector RandomPoint;
	RandomPoint.X = FMath::FRandRange(Start.X - 20000.f, Start.X + 20000.f);
	RandomPoint.Y = FMath::FRandRange(Start.Y - 20000.f, Start.Y + 20000.f);
	RandomPoint.Z = FMath::FRandRange(Start.Z, Start.Z + 10000.f);

	Flight->SetAirTarget(RandomPoint);

	return EBTNodeResult::Succeeded;
}