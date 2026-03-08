// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "FlightSystem/DragonBaseAI.h"
#include "DragonAbility.generated.h"

/*class ADragonBaseAI;*/

UCLASS(Blueprintable, BlueprintType)
class FLIGHTSYSTEM_API UDragonAbility : public UObject
{
	GENERATED_BODY()
	public:
    
    	/* Called when ability starts */
    	virtual void Start(ADragonBaseAI* InOwner, AActor* InTarget);
    
    	/* Called every tick while ability is active */
    	virtual void Tick(float DeltaTime);
    
    	/* Returns true when ability is finished */
    	virtual bool IsFinished() const;
    
    	/* Optional hook (for damage / hit events) */
    	virtual void OnAbilityHit(AActor* OtherActor, UPrimitiveComponent* OtherComp) {}
    
    protected:
    
    	// Owner dragon executing the ability 
    	UPROPERTY()
    	ADragonBaseAI* OwnerDragon = nullptr;
    
    	/* Target actor */
    	UPROPERTY()
    	AActor* TargetActor = nullptr;
    
    	/* Ability completion flag */
    	bool bFinished = false;
    
    public:
    
    	/* Useful for spawn-based abilities (fire rain etc.) */
    	int32 SpawnedCount = 0;
    
    	int32 MaxSpawns = 3;
    };