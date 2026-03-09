// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_PerchAndWatch.h"

#include "AIController.h"
#include "DragonBaseAI.h"
#include "Kismet/GameplayStatics.h"
#include "DragonFlightComponent.h"
#include "DragonPerchPoint.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DragonBaseAI.h"
#include "DragonFlightComponent.h"
#include "AIController.h"
#include "AI/Ability/DragonAbility.h"
#include "AI/Component/DragonAbilityComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"


UBTTask_PerchAndWatch::UBTTask_PerchAndWatch()
{
    bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_PerchAndWatch::ExecuteTask(
    UBehaviorTreeComponent& OwnerComp,
    uint8* NodeMemory)
{
    AAIController* AI = OwnerComp.GetAIOwner();
    if(!AI) return EBTNodeResult::Failed;

    ADragonBaseAI* Dragon =
        Cast<ADragonBaseAI>(AI->GetPawn());

    if(!Dragon) return EBTNodeResult::Failed;

    UDragonAbilityComponent* AbilityComp =
        Dragon->FindComponentByClass<UDragonAbilityComponent>();

    if(!AbilityComp || !AbilityClass)
        return EBTNodeResult::Failed;

    UDragonAbility* Ability =
        NewObject<UDragonAbility>(Dragon, AbilityClass);

    AbilityComp->StartAbility(Ability, nullptr);

    return EBTNodeResult::InProgress;
}

/*void UBTTask_PerchAndWatch::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    ADragonBaseAI* Dragon = Cast<ADragonBaseAI>(OwnerComp.GetAIOwner()->GetPawn());
    if (!Dragon) return;

    FVector Loc = Dragon->GetActorLocation();
    FVector Target = Dragon->StartLocation; // your perch ground target

    float ZDiff = Loc.Z - Target.Z;

    // Glide down smoothly
    if (ZDiff > 180.f)
    {
        float GlideSpeed = 800.f; // 👈 cinematic landing speed
        Loc.Z -= GlideSpeed * DeltaSeconds;
        Dragon->SetActorLocation(Loc);
        return;
    }

    // Final snap & land
    Loc.Z = Target.Z;
    Dragon->SetActorLocation(Loc);

    Dragon->IsFlying = false;

    UE_LOG(LogTemp, Warning, TEXT("🐉 Dragon LANDED"));

    FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
}*/

void UBTTask_PerchAndWatch::TickTask(
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