// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Component/DragonAIBehaviourComponent.h"

UDragonAIBehaviourComponent::UDragonAIBehaviourComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UDragonAIBehaviourComponent::BeginPlay()
{
	Super::BeginPlay();
}
