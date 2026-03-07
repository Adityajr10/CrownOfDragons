// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_PlayerOrbitStrafe.h"
#include "DragonBaseAI.h"
#include "DragonFlightComponent.h"
#include "DragonFireBreathComponent.h"
#include "AIController.h"
#include "Kismet/GameplayStatics.h"

UBTTask_PlayerOrbitStrafe::UBTTask_PlayerOrbitStrafe()
{
    bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_PlayerOrbitStrafe::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    UE_LOG(LogTemp, Warning, TEXT("ORBIT STRAFE STARTED"));
    return EBTNodeResult::InProgress;
}

void UBTTask_PlayerOrbitStrafe::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    ADragonBaseAI* Dragon = Cast<ADragonBaseAI>(OwnerComp.GetAIOwner()->GetPawn());
    if (!Dragon)
    {
        UE_LOG(LogTemp, Error, TEXT("Dragon invalid"));
        return;
    }

    UDragonFlightComponent* Flight = Dragon->FindComponentByClass<UDragonFlightComponent>();
    if (!Flight)
    {
        UE_LOG(LogTemp, Error, TEXT("No FlightComponent"));
        return;
    }

    UDragonFireBreathComponent* Fire = Dragon->FindComponentByClass<UDragonFireBreathComponent>();
    if (!Fire)
    {
        UE_LOG(LogTemp, Error, TEXT("No FireBreathComponent"));
    }

    AActor* Player = UGameplayStatics::GetPlayerPawn(Dragon, 0);
    if (!Player)
    {
        UE_LOG(LogTemp, Error, TEXT("Player not found"));
        return;
    }

    FVector ToDragon = (Dragon->GetActorLocation() - Player->GetActorLocation()).GetSafeNormal();
    FVector Side = FVector::CrossProduct(ToDragon, FVector::UpVector);

    float OrbitRadius = 3500.f;
    float Height = 1500.f;

    FVector Target = Player->GetActorLocation() + Side * OrbitRadius;
    Target.Z += Height;

    Flight->SetAirTarget(Target);

    if (Fire)
    {
        UE_LOG(LogTemp, Warning, TEXT("STARTING FIRE"));
        Fire->StartFire(Player);
    }
}

void UBTTask_PlayerOrbitStrafe::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
    UE_LOG(LogTemp, Warning, TEXT("ORBIT STRAFE STOPPED"));

    ADragonBaseAI* Dragon = Cast<ADragonBaseAI>(OwnerComp.GetAIOwner()->GetPawn());
    if (!Dragon) return;

    UDragonFireBreathComponent* Fire = Dragon->FindComponentByClass<UDragonFireBreathComponent>();
    if (Fire)
    {
        Fire->StopFire();
    }
}