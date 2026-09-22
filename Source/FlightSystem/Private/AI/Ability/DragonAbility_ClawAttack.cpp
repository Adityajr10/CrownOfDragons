#include "AI/Ability/DragonAbility_ClawAttack.h"

void UDragonAbility_ClawAttack::Start(ADragonBaseAI* InOwner, AActor* InTarget)
{
	Super::Start(InOwner, InTarget);
	AbilityType = EDragonAbilityType::ClawAttack;
	bAttackStarted = false;
	bGlideStarted = false;
	bFinished = false;
}

void UDragonAbility_ClawAttack::Tick(float DeltaTime)
{
	if (!OwnerDragon || !TargetActor) return;

	UDragonFlightComponent* Flight =
		OwnerDragon->FindComponentByClass<UDragonFlightComponent>();

	if (!Flight) return;

	FVector DragonLoc = OwnerDragon->GetActorLocation();
	FVector PlayerLoc = TargetActor->GetActorLocation();

	float Dist = FVector::Dist(DragonLoc, PlayerLoc);

	/* Phase 1: Approach player */
	if (!bAttackStarted && Dist > AttackDistance)
	{
		Flight->SetAirTarget(PlayerLoc);
		return;
	}

	/* Phase 2: Start glide attack */
	if (!bAttackStarted)
	{
		bAttackStarted = true;

		FVector Forward = OwnerDragon->GetActorForwardVector();

		FVector GlideDir =
			(Forward + FVector(0, 0, GlideUpStrength)).GetSafeNormal();

		GlideTarget = DragonLoc + GlideDir * GlideDistance;

		Flight->SetAirTarget(GlideTarget);

		if (ClawAttackMontage)
		{
			OwnerDragon->PlayAnimMontage(ClawAttackMontage);
		}

		return;
	}

	/* Phase 3: Continue glide */
	if (!bGlideStarted)
	{
		bGlideStarted = true;
		Flight->SetAirTarget(GlideTarget);
	}

	/* Phase 4: Finish after glide completes */
	if (bGlideStarted)
	{
		GlideTimer += DeltaTime;

		// Wait for glide to complete (minimum duration)
		if (GlideTimer < MinGlideDuration) return;

		// If montage exists, also wait for it to finish
		if (ClawAttackMontage)
		{
			if (OwnerDragon->GetMesh()->GetAnimInstance()->Montage_IsPlaying(ClawAttackMontage))
				return;
		}

		bFinished = true;
	}
}

bool UDragonAbility_ClawAttack::IsFinished() const
{
	return bFinished;
}

// Phase 1 (approaching): YES — hasn't committed to the attack yet
// Phase 2-4 (attacking + glide + montage): NO — mid-strike
bool UDragonAbility_ClawAttack::CanBeInterrupted() const
{
	return !bAttackStarted;
}

void UDragonAbility_ClawAttack::Abort(EDragonInterruptReason Reason)
{
	// Stop montage if it was started
	if (bAttackStarted && OwnerDragon && ClawAttackMontage)
	{
		OwnerDragon->StopAnimMontage(ClawAttackMontage);
	}

	Super::Abort(Reason);
}