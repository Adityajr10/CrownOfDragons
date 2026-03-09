// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Component/DragonAIBehaviourComponent.h"

UDragonAIBehaviourComponent::UDragonAIBehaviourComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UDragonAIBehaviourComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentEnergy = MaxEnergy;
}

bool UDragonAIBehaviourComponent::CanUseAbility(const FDragonAbilityData& AbilityData) const
{
	// Check Energy 
	if (CurrentEnergy < AbilityData.EnergyCost)
	{
		return false;
	}
	/* Check Cooldown */
	const float* LastTime = LastAbilityUseTime.Find(AbilityData.AbilityType);

	if (LastTime)
	{
		float CurrentTime = GetWorld()->GetTimeSeconds();

		if (CurrentTime - *LastTime < AbilityData.Cooldown)
		{
			return false;
		}
	}

	return true;
}

void UDragonAIBehaviourComponent::ConsumeEnergy(float Amount)
{
	CurrentEnergy -= Amount;

	CurrentEnergy = FMath::Clamp(CurrentEnergy, 0.f, MaxEnergy);
}

void UDragonAIBehaviourComponent::RegenerateEnergy(float DeltaTime)
{
	CurrentEnergy += EnergyRegenRate * DeltaTime;

	CurrentEnergy = FMath::Clamp(CurrentEnergy, 0.f, MaxEnergy);
}

void UDragonAIBehaviourComponent::UseAbility(const FDragonAbilityData& AbilityData)
{
	float CurrentTime = GetWorld()->GetTimeSeconds();

	// Record last usage time 
	LastAbilityUseTime.Add(AbilityData.AbilityType, CurrentTime);

	/* Consume energy */
	ConsumeEnergy(AbilityData.EnergyCost);

	/* Update current ability */
	CurrentAbility = AbilityData.AbilityType;
}