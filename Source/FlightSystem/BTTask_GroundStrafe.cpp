// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_GroundStrafe.h"

#include "AIController.h"
#include "DragonBaseAI.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

struct FStrafeMemory
{
    FVector Start;
    FVector End;
    bool bFinished = false;
};

UBTTask_GroundStrafe::UBTTask_GroundStrafe()
{
    bNotifyTick = true;
    NodeName = "Ground Strafe";
}

uint16 UBTTask_GroundStrafe::GetInstanceMemorySize() const
{
    return sizeof(FStrafeMemory);
}

EBTNodeResult::Type UBTTask_GroundStrafe::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    ADragonBaseAI* Dragon = Cast<ADragonBaseAI>(OwnerComp.GetAIOwner()->GetPawn());
    if (!Dragon) return EBTNodeResult::Failed;

    AActor* Player = UGameplayStatics::GetPlayerPawn(Dragon, 0);
    if (!Player) return EBTNodeResult::Failed;

    FStrafeMemory* Mem = (FStrafeMemory*)NodeMemory;

    FVector Dir = Dragon->GetActorForwardVector();

    FVector GroundStart = Player->GetActorLocation();
    GroundStart.Z = Player->GetActorLocation().Z + Dragon->StrafeHeight;

    Mem->Start = GroundStart;
    Mem->End = GroundStart + Dir * Dragon->StrafeDistance;

    Dragon->SetActorLocation(Mem->Start);
    Dragon->SetActorRotation(Dir.Rotation());

    if (Dragon->StrafeMontage)
        Dragon->PlayAnimMontage(Dragon->StrafeMontage);

    return EBTNodeResult::InProgress;
}

void UBTTask_GroundStrafe::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    FStrafeMemory* Mem = (FStrafeMemory*)NodeMemory;
    ADragonBaseAI* Dragon = Cast<ADragonBaseAI>(OwnerComp.GetAIOwner()->GetPawn());
    if (!Dragon) return;

    FVector Dir = (Mem->End - Mem->Start).GetSafeNormal();

    // REAL flying movement
    Dragon->GetCharacterMovement()->SetMovementMode(MOVE_Flying);
    Dragon->GetCharacterMovement()->Velocity = Dir * Dragon->StrafeSpeed;

    // Face movement
    Dragon->SetActorRotation(Dir.Rotation());

    // Continuous fire carpet
    Dragon->FireBreath->StartFire(UGameplayStatics::GetPlayerPawn(Dragon, 0));

    // Finish when reached end
    if (FVector::Dist(Dragon->GetActorLocation(), Mem->End) < 300.f)
    {
        Dragon->GetCharacterMovement()->StopMovementImmediately();
        Dragon->FireBreath->StopFire();
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
    }
}
