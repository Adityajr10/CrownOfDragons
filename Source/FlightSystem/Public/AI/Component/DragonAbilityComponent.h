// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DragonAbilityComponent.generated.h"


class UDragonAbility;
class ADragonBaseAI;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FLIGHTSYSTEM_API UDragonAbilityComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDragonAbilityComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent(
		float DeltaTime,
		ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;
	
	void StartAbility(UDragonAbility* Ability, AActor* Target);
	
	bool IsAbilityActive() const;

protected:

	UPROPERTY()
	ADragonBaseAI* OwnerDragon = nullptr;

	UPROPERTY()
	UDragonAbility* ActiveAbility = nullptr;
};
