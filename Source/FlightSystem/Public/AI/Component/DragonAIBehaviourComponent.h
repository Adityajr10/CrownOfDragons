// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

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
};