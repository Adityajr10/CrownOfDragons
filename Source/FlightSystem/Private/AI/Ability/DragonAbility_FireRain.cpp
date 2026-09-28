#include "AI/Ability/DragonAbility_FireRain.h"
#include "AI/AbilityActor/DragonFireActor.h"

void UDragonAbility_FireRain::SetTargetLocation(const FVector& InLocation)
{
    TargetLocation = InLocation;
}

void UDragonAbility_FireRain::Start(
    ADragonBaseAI* InOwner,
    AActor* InTarget)
{
    Super::Start(InOwner, InTarget);
    AbilityType = EDragonAbilityType::FireRain;
    bReachedLocation = false;
    bMontageStarted = false;
    bFireSpawned = false;
    DelayTimer = 0.f;
}

void UDragonAbility_FireRain::Tick(float DeltaTime)
{
    if (!OwnerDragon) return;

    UDragonFlightComponent* Flight =
       OwnerDragon->FindComponentByClass<UDragonFlightComponent>();

    if (!Flight) return;

    /* Phase 1: Move dragon above player — INTERRUPTIBLE */

    if (!bReachedLocation)
    {
       FVector PlayerLoc = TargetActor->GetActorLocation();
       FVector AirPos = PlayerLoc + FVector(0,0,FireRainHeight);

       Flight->SetAirTarget(AirPos);

       float Dist =
          FVector::Dist(
             OwnerDragon->GetActorLocation(),
             AirPos);

       if (Dist < ReachDistance)
       {
          bReachedLocation = true;
       }

       return;
    }

    /* Phase 2: Start montage — NOT INTERRUPTIBLE (committed to animation) */
    if (!bMontageStarted)
    {
       bMontageStarted = true;

       if (FireRainMontage)
       {
          OwnerDragon->PlayAnimMontage(FireRainMontage);
       }

       return;
    }

    /* Phase 3: Delay before fire spawn — NOT INTERRUPTIBLE (mid-cast) */
    if (!bFireSpawned)
    {
       DelayTimer += DeltaTime;

       if (DelayTimer < FireDelay) return;

       bFireSpawned = true;

       if (!FireActorClass) return;

       FVector SpawnLoc =
    OwnerDragon->GetMesh()->GetSocketLocation("Tongue");

       FVector PlayerLoc =
          TargetActor->GetActorLocation();

       FVector Dir =
          (PlayerLoc - SpawnLoc).GetSafeNormal();

       FRotator Rot = Dir.Rotation();

       UWorld* World = OwnerDragon->GetWorld();

       if (!World) return;

       ADragonFireActor* Fire =
          World->SpawnActor<ADragonFireActor>(
             FireActorClass,
             SpawnLoc,
             Rot);

       if (Fire)
       {
          Fire->Init(Dir);
       }

       bFinished = true;
    }
}

bool UDragonAbility_FireRain::IsFinished() const
{
    return bFinished;
}

// ============================================================
// NEW: Phase-dependent interruptibility
// ============================================================

bool UDragonAbility_FireRain::CanBeInterrupted() const
{
    // Phase 1 (flying to position): YES — hasn't committed yet,
    // dragon is just flying, safe to abort and reposition
    if (!bReachedLocation)
    {
        return true;
    }

    // Phase 2 (montage playing): NO — mid-animation, interrupting
    // would look broken and leave the montage in a bad state
    // Phase 3 (fire delay / spawning): NO — already casting,
    // let it finish naturally
    return false;
}

void UDragonAbility_FireRain::Abort(EDragonInterruptReason Reason)
{
    // Clean up based on what phase we're in

    if (!bReachedLocation)
    {
        // Phase 1: just flying — nothing to clean up
        UE_LOG(LogTemp, Log, TEXT("FireRain aborted during positioning"));
    }
    else if (bMontageStarted && !bFireSpawned)
    {
        // Phase 2/3: montage is playing — stop it cleanly
        if (OwnerDragon && FireRainMontage)
        {
            OwnerDragon->StopAnimMontage(FireRainMontage);
        }
        UE_LOG(LogTemp, Log, TEXT("FireRain aborted during montage — stopped montage"));
    }
    // If bFireSpawned is true, the fire actor is already spawned
    // and self-destructs on its own — no cleanup needed

    // Always call Super — sets bAborted and bFinished
    Super::Abort(Reason);
}

void UDragonAbility_FireRain::OnOwnerDamaged(
    float DamageAmount, float DamagePercent, AActor* Instigator)
{
    // Optional: play a flinch reaction during phase 1
    // This doesn't abort — just a visual response
    if (!bReachedLocation && OwnerDragon)
    {
        // Could play a small flinch montage here if you have one
        // OwnerDragon->PlayAnimMontage(FlinchMontage);
        UE_LOG(LogTemp, Log,
            TEXT("FireRain: took damage while positioning (%.0f%%)"),
            DamagePercent * 100.f);
    }
}