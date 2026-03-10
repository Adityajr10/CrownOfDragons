// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Components/ActorComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "AI/Data/DragonAITypes.h"
#include "AI/Data/DragonAbilityData.h"
#include "AI/Data/DragonPersonalitySettings.h"
#include "DragonAIBehaviourComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FLIGHTSYSTEM_API UDragonAIBehaviourComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UDragonAIBehaviourComponent();

protected:
	virtual void BeginPlay() override;

public:

	UPROPERTY()
	AAIController* OwnerAIController;
	UPROPERTY()
	UBlackboardComponent* BlackboardComponent;
	UPROPERTY()
	AActor* TargetActor = nullptr;
	
	//Personality configuration 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	FDragonPersonalitySettings PersonalitySettings;

	// Abilities this dragon can use 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	TArray<FDragonAbilityData> AbilitySettings;

	// Current AI State 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI")
	EDragonState CurrentState = EDragonState::Idle;

	// Current selected ability
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI")
	EDragonAbilityType CurrentAbility = EDragonAbilityType::None;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI")
	EDragonInstinct CurrentInstinct = EDragonInstinct::Roaming;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Ability")
	EDragonInstinct InstinctType = EDragonInstinct::Attacking;
	
	
	//Energy System

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Energy")
	float MaxEnergy = 100.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Energy")
	float CurrentEnergy = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Energy")
	float EnergyRegenRate = 10.f;
	
	void ConsumeEnergy(float Amount);

	void RegenerateEnergy(float DeltaTime);

	//Cooldown Tracking 

	UPROPERTY()
	TMap<EDragonAbilityType, float> LastAbilityUseTime;

	//Validation 
	bool CanUseAbility(const FDragonAbilityData& AbilityData) const;

	//Ability Usage 
	void UseAbility(const FDragonAbilityData& AbilityData);
	
	// AI Brain
	void EvaluateSituation();
	void SelectInstinct();
	void SelectAbility();
	float GetDistanceToTarget() const;
	void UpdateBlackboard();
	
	//Ability Memory 

	UPROPERTY()
	EDragonAbilityType LastUsedAbility = EDragonAbilityType::None;

	UPROPERTY()
	EDragonAbilityType SecondLastUsedAbility = EDragonAbilityType::None;

	//Combat Timing

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat")
	float MinAttackInterval = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat")
	float MaxAttackInterval = 4.0f;

	UPROPERTY()
	float NextAttackTime = 0.f;
	bool CanPerformAttack() const;
	
	// Instinct Persistence

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	float MinInstinctDuration = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	float MaxInstinctDuration = 5.0f;
	
	float NextInstinctChangeTime = 0.f;
	
	//AiController target set
	void SetTargetActor(AActor* NewTarget);
};