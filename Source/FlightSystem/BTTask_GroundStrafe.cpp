// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_GroundStrafe.h"

#include "AIController.h"
#include "DragonBaseAI.h"
#include "AI/Component/DragonAbilityComponent.h"
#include "AI/Ability/DragonAbility.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

struct FStrafeMemory
{
    FVector Start;
    FVector End;
    bool bFinished = false;
};

uint16 UBTTask_GroundStrafe::GetInstanceMemorySize() const
{
    return sizeof(FStrafeMemory);
}
UBTTask_GroundStrafe::UBTTask_GroundStrafe()
{
    NodeName = "Ground Strafe";
    bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_GroundStrafe::ExecuteTask(
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

void UBTTask_GroundStrafe::TickTask(
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