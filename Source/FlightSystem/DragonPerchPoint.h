// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DragonPerchPoint.generated.h"

UCLASS()
class FLIGHTSYSTEM_API ADragonPerchPoint : public AActor
{
	GENERATED_BODY()
	
public:	

	ADragonPerchPoint();

	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;
};
