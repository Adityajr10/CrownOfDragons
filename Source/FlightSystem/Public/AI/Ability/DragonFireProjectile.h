// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/Ability/DragonAbility.h"
#include "DragonFireProjectile.generated.h"


class ADragonFireActor;
UCLASS(Blueprintable)
class FLIGHTSYSTEM_API UDragonFireProjectile : public UDragonAbility
{
	GENERATED_BODY()
	
public:

	virtual void Start(ADragonBaseAI* InOwner, AActor* InTarget) override;
	virtual void Tick(float DeltaTime) override;

protected:

	UPROPERTY(EditDefaultsOnly, Category="Fire")
	TSubclassOf<ADragonFireActor> FireActorClass;

	UPROPERTY(EditDefaultsOnly, Category="Fire")
	float SpawnInterval = 1.5f;

	UPROPERTY(EditDefaultsOnly, Category="Movement")
	float MoveRadius = 1500.f;

	UPROPERTY(EditDefaultsOnly, Category="Movement")
	float MoveHeight = 800.f;

	float SpawnTimer = 0.f;
	
	FVector CurrentMoveTarget;
	bool bHasMoveTarget = false;
};