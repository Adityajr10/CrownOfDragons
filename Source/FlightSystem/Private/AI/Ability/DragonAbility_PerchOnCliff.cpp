#include "AI/Ability/DragonAbility_PerchOnCliff.h"
#include "GameFramework/CharacterMovementComponent.h"

void UDragonAbility_PerchOnCliff::SetPerchLocation(const FVector& InLocation)
{
    //PerchLocation = InLocation;
   // PerchLocation = FVector(61613.709893, 3459.903335, 7779.841142);
  PerchLocation =  FVector(36988.972731, 22267.368020, 7439.448210);
}

void UDragonAbility_PerchOnCliff::Start(
    ADragonBaseAI* InOwner,
    AActor* InTarget)
{
    Super::Start(InOwner, InTarget);
    AbilityType = EDragonAbilityType::PerchOnCliff;
    bLandingStarted  = false;
    bMontageFinished = false;
    bFinished        = false;
    PostLandTimer    = 0.f;
}

void UDragonAbility_PerchOnCliff::Tick(float DeltaTime)
{
    if (!OwnerDragon) return;

    UDragonFlightComponent* Flight =
        OwnerDragon->FindComponentByClass<UDragonFlightComponent>();

    if (!Flight) return;

    if (!bLandingStarted)
    {
        Flight->SetAirTarget(PerchLocation);

        float Dist = FVector::Dist(
            OwnerDragon->GetActorLocation(),
            PerchLocation);

        if (Dist < LandingDistance)
        {
            bLandingStarted = true;

            if (LandingMontage)
                OwnerDragon->PlayAnimMontage(LandingMontage);

            OwnerDragon->IsFlying = false;
        }

        return;
    }

    if (!bMontageFinished)
    {
        UAnimInstance* Anim = OwnerDragon->GetMesh()->GetAnimInstance();

        if (Anim && LandingMontage && Anim->Montage_IsPlaying(LandingMontage))
            return; 

        bMontageFinished = true;
        PostLandTimer    = 0.f;
        return;
    }
    PostLandTimer += DeltaTime;

    if (PostLandTimer >= PostLandWaitDuration)
    {
        bFinished = true;
    }
}

bool UDragonAbility_PerchOnCliff::IsFinished() const
{
    return bFinished;
}