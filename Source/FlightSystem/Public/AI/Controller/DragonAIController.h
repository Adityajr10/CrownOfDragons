// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Controller.h"
#include "DragonAIController.generated.h"


class UBehaviorTree;
class UBehaviorTreeComponent;
class UBlackboardComponent;
UCLASS()
class FLIGHTSYSTEM_API ADragonAIController : public AController
{
	GENERATED_BODY()
public:

	ADragonAIController();

protected:

	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	UBehaviorTree* BehaviorTree;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI")
	UBehaviorTreeComponent* BehaviorTreeComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI")
	UBlackboardComponent* BlackboardComponent;
};