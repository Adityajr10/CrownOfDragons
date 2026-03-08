// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Component/DragonAbilityComponent.h"
#include "AI/Ability/DragonAbility.h"
#include "FlightSystem/DragonBaseAI.h"

UDragonAbilityComponent::UDragonAbilityComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UDragonAbilityComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerDragon = Cast<ADragonBaseAI>(GetOwner());
}

void UDragonAbilityComponent::TickComponent(
	float DeltaTime,
	ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (ActiveAbility)
	{
		ActiveAbility->Tick(DeltaTime);

		if (ActiveAbility->IsFinished())
		{
			ActiveAbility = nullptr;
		}
	}
}

void UDragonAbilityComponent::StartAbility(UDragonAbility* Ability, AActor* Target)
{
	if (!Ability || ActiveAbility)
	{
		return;
	}

	ActiveAbility = Ability;
	ActiveAbility->Start(OwnerDragon, Target);
}

bool UDragonAbilityComponent::IsAbilityActive() const
{
	return ActiveAbility != nullptr;
}

