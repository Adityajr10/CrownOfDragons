// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_PlayerOrbitStrafe.h"
#include "DragonBaseAI.h"
#include "DragonFlightComponent.h"
#include "DragonFireBreathComponent.h"
#include "AIController.h"
#include "AI/Ability/DragonAbility.h"
#include "AI/Component/DragonAbilityComponent.h"
#include "Kismet/GameplayStatics.h"

UBTTask_PlayerOrbitStrafe::UBTTask_PlayerOrbitStrafe()
{
    bNotifyTick = true;
}
EBTNodeResult::Type UBTTask_PlayerOrbitStrafe::ExecuteTask(
    UBehaviorTreeComponent& OwnerComp,
    uint8* NodeMemory)
{
    AAIController* AI = OwnerComp.GetAIOwner();
    if (!AI) return EBTNodeResult::Failed;

    ADragonBaseAI* Dragon =
        Cast<ADragonBaseAI>(AI->GetPawn());

    if (!Dragon) return EBTNodeResult::Failed;

    UDragonAbilityComponent* AbilityComp =
        Dragon->FindComponentByClass<UDragonAbilityComponent>();

    if (!AbilityComp || !AbilityClass)
        return EBTNodeResult::Failed;

    AActor* Player =
        UGameplayStatics::GetPlayerPawn(Dragon, 0);

    UDragonAbility* Ability =
        NewObject<UDragonAbility>(Dragon, AbilityClass);

    AbilityComp->StartAbility(Ability, Player);

    return EBTNodeResult::InProgress;
}

void UBTTask_PlayerOrbitStrafe::TickTask(
    UBehaviorTreeComponent& OwnerComp,
    uint8* NodeMemory,
    float DeltaSeconds)
{
    AAIController* AI = OwnerComp.GetAIOwner();
    if (!AI) return;

    ADragonBaseAI* Dragon =
        Cast<ADragonBaseAI>(AI->GetPawn());

    if (!Dragon) return;

    UDragonAbilityComponent* AbilityComp =
        Dragon->FindComponentByClass<UDragonAbilityComponent>();

    if (!AbilityComp->IsAbilityActive())
    {
        FinishLatentTask(
            OwnerComp,
            EBTNodeResult::Succeeded
        );
    }
}

/*
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
}*/