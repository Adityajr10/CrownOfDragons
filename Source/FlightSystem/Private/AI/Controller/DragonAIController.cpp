#include "AI/Controller/DragonAIController.h"

#include "AI/Component/DragonAIBehaviourComponent.h"
#include "FlightSystem/DragonBaseAI.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionComponent.h"

ADragonAIController::ADragonAIController()
{
	BehaviorTreeComponent =
		CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));

	BlackboardComponent =
		CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));

	// Create perception component
	UAIPerceptionComponent* Perception =
		CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));

	SetPerceptionComponent(*Perception);

	SightConfig =
		CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));

	SightConfig->SightRadius = 40000.f;
	SightConfig->LoseSightRadius = 45000.f;
	SightConfig->PeripheralVisionAngleDegrees = 120.f;

	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

	Perception->ConfigureSense(*SightConfig);
	Perception->SetDominantSense(SightConfig->GetSenseImplementation());
}

void ADragonAIController::BeginPlay()
{
	Super::BeginPlay();

	if (GetPerceptionComponent())
	{
		GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(
			this,
			&ADragonAIController::OnTargetDetected
		);
	}
}

void ADragonAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (BehaviorTree)
	{
		// ✅ Use correct function
		RunBehaviorTree(BehaviorTree);
	}

	// Inject Blackboard into BehaviourComponent

	ADragonBaseAI* Dragon = Cast<ADragonBaseAI>(InPawn);
	if (!Dragon) return;

	UDragonAIBehaviourComponent* Behaviour =
		Dragon->FindComponentByClass<UDragonAIBehaviourComponent>();

	if (Behaviour)
	{
		Behaviour->SetBlackboard(GetBlackboardComponent()); // ✅ IMPORTANT

		UE_LOG(LogTemp, Warning, TEXT("Blackboard injected into BehaviourComponent"));
	}
}

void ADragonAIController::OnTargetDetected(AActor* Actor, FAIStimulus Stimulus)
{
	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn) return;

	UDragonAIBehaviourComponent* Behaviour =
		ControlledPawn->FindComponentByClass<UDragonAIBehaviourComponent>();

	if (!Behaviour) return;

	if (Stimulus.WasSuccessfullySensed())
	{
		if (Behaviour->IsValidTarget(Actor))
		{
			//UE_LOG(LogTemp, Warning, TEXT("Dragon detected target: %s"), *Actor->GetName());

			Behaviour->SetTargetActor(Actor);
		}
	}
	else
	{
		//UE_LOG(LogTemp, Warning, TEXT("Dragon lost target"));

		Behaviour->SetTargetActor(nullptr);
	}
}