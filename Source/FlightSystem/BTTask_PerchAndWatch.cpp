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
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"


UBTTask_PerchAndWatch::UBTTask_PerchAndWatch()
{
    bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_PerchAndWatch::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    UE_LOG(LogTemp, Warning, TEXT("🐉 PERCH TASK STARTED"));

    ADragonBaseAI* Dragon = Cast<ADragonBaseAI>(OwnerComp.GetAIOwner()->GetPawn());
    if (!Dragon)
    {
        UE_LOG(LogTemp, Error, TEXT("No Dragon pawn"));
        return EBTNodeResult::Failed;
    }

    // Pick random area around start location
    FVector Origin = Dragon->StartLocation;
    FVector RandomXY;
    RandomXY.X = FMath::FRandRange(Origin.X - 15000.f, Origin.X + 15000.f);
    RandomXY.Y = FMath::FRandRange(Origin.Y - 15000.f, Origin.Y + 15000.f);
    RandomXY.Z = Origin.Z + 8000.f; // high in air

    FHitResult Hit;
    FVector TraceStart = RandomXY;
    FVector TraceEnd = RandomXY - FVector(0, 0, 20000.f);

    bool bHit = Dragon->GetWorld()->LineTraceSingleByChannel(
        Hit, TraceStart, TraceEnd, ECC_Visibility);

    if (!bHit)
    {
        UE_LOG(LogTemp, Error, TEXT("No ground hit found"));
        return EBTNodeResult::Failed;
    }

    FVector PerchPoint = Hit.ImpactPoint;
    PerchPoint.Z += 150.f; // hover slightly above ground

    UE_LOG(LogTemp, Warning, TEXT("Perch found at %s"), *PerchPoint.ToString());

    UDragonFlightComponent* Flight = Dragon->FindComponentByClass<UDragonFlightComponent>();
    if (!Flight)
    {
        UE_LOG(LogTemp, Error, TEXT("No FlightComponent"));
        return EBTNodeResult::Failed;
    }

    // Fly down to perch
    Dragon->GetCharacterMovement()->MaxFlySpeed = 3000.f;
    Dragon->GetCharacterMovement()->BrakingDecelerationFlying = 5000.f;

    Flight->SetAirTarget(PerchPoint);

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

void UBTTask_PerchAndWatch::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    ADragonBaseAI* Dragon = Cast<ADragonBaseAI>(OwnerComp.GetAIOwner()->GetPawn());
    if (!Dragon) return;

    FVector Loc = Dragon->GetActorLocation();
    FVector Target = Dragon->StartLocation;   // Your ground perch target

    float ZDiff = Loc.Z - Target.Z;

    // Smooth glide descent
    if (ZDiff > 180.f)
    {
        float GlideSpeed = 800.f;
        Loc.Z -= GlideSpeed * DeltaSeconds;
        Dragon->SetActorLocation(Loc);
        return;
    }

    // Final snap & perch
    Loc.Z = Target.Z;
    Dragon->SetActorLocation(Loc);

    Dragon->IsFlying = false;

    UE_LOG(LogTemp, Warning, TEXT("🐉 Dragon LANDED"));

    // Wing fold
    if (Dragon->WingFoldMontage)
    {
        Dragon->PlayAnimMontage(Dragon->WingFoldMontage);
    }

    // Perch roar loop
    FTimerHandle RoarTimer;
    Dragon->GetWorldTimerManager().SetTimer(RoarTimer, [Dragon]()
    {
        if (!Dragon->IsFlying && Dragon->PerchRoarMontage)
        {
            Dragon->PlayAnimMontage(Dragon->PerchRoarMontage);
        }
    }, 3.f, true);

    // Auto takeoff after watch time
    FTimerHandle TakeoffTimer;
    Dragon->GetWorldTimerManager().SetTimer(TakeoffTimer, [Dragon]()
    {
        if (!Dragon->IsFlying)
        {
            Dragon->TakeoffFromPerch();
        }
    }, Dragon->PerchWatchTime, false);

    FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
}
