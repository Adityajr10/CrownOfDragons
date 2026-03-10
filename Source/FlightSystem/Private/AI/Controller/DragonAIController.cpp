// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Controller/DragonAIController.h"

#include "AI/Component/DragonAIBehaviourComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"

ADragonAIController::ADragonAIController()
{
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));
	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));
	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent"));

	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));

	SightConfig->SightRadius = 40000.f;
	SightConfig->LoseSightRadius = 45000.f;
	SightConfig->PeripheralVisionAngleDegrees = 120.f;

	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

	PerceptionComponent->ConfigureSense(*SightConfig);
	PerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());
}

void ADragonAIController::BeginPlay()
{
	Super::BeginPlay();
	PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(
	this,
	&ADragonAIController::OnTargetDetected
);
}

void ADragonAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (BehaviorTree)
	{
		BlackboardComponent->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
		BehaviorTreeComponent->StartTree(*BehaviorTree);
	}
}

void ADragonAIController::OnTargetDetected(AActor* Actor, FAIStimulus Stimulus)
{
	APawn* ControlledPawn = GetPawn();

	if (!ControlledPawn)
	{
		return;
	}

	UDragonAIBehaviourComponent* Behaviour =
		ControlledPawn->FindComponentByClass<UDragonAIBehaviourComponent>();

	if (!Behaviour)
	{
		return;
	}

	if (Stimulus.WasSuccessfullySensed())
	{
		if (Behaviour->IsValidTarget(Actor))
		{
			Behaviour->SetTargetActor(Actor);
		}
	}
	else
	{
		Behaviour->SetTargetActor(nullptr);
	}
}