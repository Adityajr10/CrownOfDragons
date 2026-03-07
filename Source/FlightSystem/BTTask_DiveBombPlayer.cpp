// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_DiveBombPlayer.h"

#include "AIController.h"
#include "DragonBaseAI.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

struct FDiveMemory
{
    FVector ClimbTarget;
    bool bReachedTop = false;
};

UBTTask_DiveBombPlayer::UBTTask_DiveBombPlayer()
{
    bNotifyTick = true;
    NodeName = "Dive Bomb Player";
}

uint16 UBTTask_DiveBombPlayer::GetInstanceMemorySize() const
{
    return sizeof(FDiveMemory);
}

EBTNodeResult::Type UBTTask_DiveBombPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    ADragonBaseAI* Dragon = Cast<ADragonBaseAI>(OwnerComp.GetAIOwner()->GetPawn());
    if (!Dragon) return EBTNodeResult::Failed;

    AActor* Player = UGameplayStatics::GetPlayerPawn(Dragon, 0);
    if (!Player) return EBTNodeResult::Failed;

    FDiveMemory* Mem = (FDiveMemory*)NodeMemory;
    Mem->bReachedTop = false;

    FVector ToPlayer = (Player->GetActorLocation() - Dragon->GetActorLocation()).GetSafeNormal();
    FVector ClimbDir = (ToPlayer + FVector::UpVector * 1.2f).GetSafeNormal();

    Dragon->SetActorRotation(ClimbDir.Rotation());
    Dragon->LaunchCharacter(ClimbDir * 4000.f, true, true);

    return EBTNodeResult::InProgress;
}


/*void UBTTask_DiveBombPlayer::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    FDiveMemory* Mem = (FDiveMemory*)NodeMemory;

    ADragonBaseAI* Dragon = Cast<ADragonBaseAI>(OwnerComp.GetAIOwner()->GetPawn());
    AActor* Player = UGameplayStatics::GetPlayerPawn(Dragon, 0);
    if (!Dragon || !Player) return;

    // At crest, switch from climb to dive
    if (!Mem->bReachedTop)
    {
        if (Dragon->GetVelocity().Z < 50.f)
        {
            Mem->bReachedTop = true;

            FVector DiveDir = (Player->GetActorLocation() - Dragon->GetActorLocation()).GetSafeNormal();
            Dragon->SetActorRotation(DiveDir.Rotation());
            Dragon->LaunchCharacter(DiveDir * Dragon->DiveSpeed, true, true);
        }
    }
  else
{
    FVector CurrentLoc = Dragon->GetActorLocation();
    FVector TargetLoc = Player->GetActorLocation();

    FVector Dir = (TargetLoc - CurrentLoc).GetSafeNormal();

    float DiveStep = Dragon->DiveSpeed * DeltaSeconds;

    FVector NewLoc = CurrentLoc + Dir * DiveStep;

    Dragon->SetActorLocation(NewLoc);
    Dragon->SetActorRotation(Dir.Rotation());

    if (FVector::Dist(NewLoc, TargetLoc) < 200.f)
    {
        Dragon->OnDiveImpact();
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
    }
}
}*/

void UBTTask_DiveBombPlayer::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    FDiveMemory* Mem = (FDiveMemory*)NodeMemory;

    ADragonBaseAI* Dragon = Cast<ADragonBaseAI>(OwnerComp.GetAIOwner()->GetPawn());
    AActor* Player = UGameplayStatics::GetPlayerPawn(Dragon, 0);
    if (!Dragon || !Player) return;

    // PHASE 1: reach crest
    if (!Mem->bReachedTop)
    {
        if (Dragon->GetVelocity().Z < 50.f)
        {
            Mem->bReachedTop = true;

            // Stop CharacterMovement influence completely
            Dragon->GetCharacterMovement()->StopMovementImmediately();
            Dragon->GetCharacterMovement()->DisableMovement();

            FVector DiveDir = (Player->GetActorLocation() - Dragon->GetActorLocation()).GetSafeNormal();
            Dragon->SetActorRotation(DiveDir.Rotation());
        }
        return;
    }

    // PHASE 2: manual homing dive
    FVector CurrentLoc = Dragon->GetActorLocation();
    FVector TargetLoc = Player->GetActorLocation();

    FVector Dir = (TargetLoc - CurrentLoc).GetSafeNormal();
    float Step = Dragon->DiveSpeed * DeltaSeconds;

    FVector NewLoc = CurrentLoc + Dir * Step;

    Dragon->SetActorLocation(NewLoc);
    Dragon->SetActorRotation(Dir.Rotation());

    // Impact
    if (FVector::Dist(NewLoc, TargetLoc) < 200.f)
    {
        Dragon->OnDiveImpact();

        // Re-enable movement so flight resumes
        Dragon->GetCharacterMovement()->SetMovementMode(MOVE_Flying);

        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
    }
}
