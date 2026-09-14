#include "AI/Component/DragonAIBehaviourComponent.h"
#include "AI/Component/DragonAbilityComponent.h"
#include "AI/Component/DragonStimulusComponent.h"
#include "AI/Data/DragonAbilityData.h"
#include "AI/Data/DragonAITypes.h"
#include "FlightSystem/DragonBaseAI.h"
#include "DrawDebugHelpers.h"

UDragonAIBehaviourComponent::UDragonAIBehaviourComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UDragonAIBehaviourComponent::BeginPlay()
{
	Super::BeginPlay();

	HomeLocation = GetOwner()->GetActorLocation();
	CurrentEnergy = MaxEnergy;

	OwnerAIController = Cast<AAIController>(GetOwner()->GetInstigatorController());

	if (OwnerAIController)
	{
		BlackboardComponent = OwnerAIController->GetBlackboardComponent();
	}

	// Subscribe to stimulus events
	UDragonStimulusComponent* Stimulus =
		GetOwner()->FindComponentByClass<UDragonStimulusComponent>();

	if (Stimulus)
	{
		Stimulus->OnStimulusReceived.AddDynamic(
			this, &UDragonAIBehaviourComponent::OnStimulusReceived);
	}
	// DEBUG: Log all ability settings on startup
	UE_LOG(LogTemp, Warning, TEXT("========== ABILITY SETTINGS DUMP (%d entries) =========="), AbilitySettings.Num());
	for (int32 i = 0; i < AbilitySettings.Num(); i++)
	{
		const FDragonAbilityData& A = AbilitySettings[i];
		UE_LOG(LogTemp, Warning,
			TEXT("[%d] %s | Instinct=%s | Dist=%.0f-%.0f | Alt=%.0f-%.0f | CD=%.1f | Energy=%.1f | Weight=%.1f | Priority=%s"),
			i,
			*UEnum::GetValueAsString(A.AbilityType),
			*UEnum::GetValueAsString(A.InstinctType),
			A.PreferredDistanceMin, A.PreferredDistanceMax,
			A.MinAltitudeDifference, A.MaxAltitudeDifference,
			A.Cooldown, A.EnergyCost, A.Weight,
			*UEnum::GetValueAsString(A.Priority));
	}
	UE_LOG(LogTemp, Warning, TEXT("====================================================="));
}

void UDragonAIBehaviourComponent::TickComponent(
	float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Energy regen (3x while resting)
	float RegenMultiplier = 1.f;
	if (CurrentInstinct == EDragonInstinct::Resting)
	{
		RegenMultiplier = RestingRegenMultiplier;

		// Auto-exit resting when energy recovered
		float EnergyPercent = MaxEnergy > 0.f ? CurrentEnergy / MaxEnergy : 1.f;
		if (EnergyPercent >= RestingExitThreshold)
		{
			UE_LOG(LogTemp, Log, TEXT("Energy recovered to %.0f%% — exiting rest"),
				EnergyPercent * 100.f);
			SelectInstinct();
			SelectAbility();
		}
	}

	CurrentEnergy += EnergyRegenRate * RegenMultiplier * DeltaTime;
	CurrentEnergy = FMath::Clamp(CurrentEnergy, 0.f, MaxEnergy);

	bReactingStimulusThisFrame = false;

	// Watchdog: brain picked an ability but no BT task ever started it
	// (usually a missing tree node for that ability) — re-roll instead of stalling
	if (!bAbilityRunning && CurrentAbility != EDragonAbilityType::None)
	{
		AbilityStallTime += DeltaTime;

		if (AbilityStallTime >= AbilityStallTimeout)
		{
			UE_LOG(LogTemp, Warning,
				TEXT("Watchdog: [%s] selected but never started after %.1fs — re-selecting (missing BT node?)"),
				*UEnum::GetValueAsString(CurrentAbility), AbilityStallTime);

			// Cooldown the stalled ability so the re-roll picks something else
			LastAbilityUseTime.Add(CurrentAbility, GetWorld()->GetTimeSeconds());

			AbilityStallTime = 0.f;
			SelectInstinct();
			SelectAbility();
		}
	}
	else
	{
		AbilityStallTime = 0.f;
	}

	if (bShowDebugInfo)
	{
		DrawOverheadDebug();
	}
}

void UDragonAIBehaviourComponent::DrawOverheadDebug() const
{
#if ENABLE_DRAW_DEBUG
	AActor* Owner = GetOwner();
	if (!Owner || !GetWorld()) return;

	UDragonAbilityComponent* AbilityComp =
		Owner->FindComponentByClass<UDragonAbilityComponent>();

	EDragonAbilityType ActiveAbility = AbilityComp
		? AbilityComp->GetActiveAbilityType()
		: EDragonAbilityType::None;

	UDragonStimulusComponent* StimulusComp =
		Owner->FindComponentByClass<UDragonStimulusComponent>();
	float HealthPercent = StimulusComp ? StimulusComp->HealthPercent : 1.f;

	FString Text = FString::Printf(
		TEXT("State: %s\nInstinct: %s\nAbility: %s\nHP: %.0f%%\nEnergy: %.0f/%.0f"),
		*UEnum::GetDisplayValueAsText(CurrentState).ToString(),
		*UEnum::GetDisplayValueAsText(CurrentInstinct).ToString(),
		*UEnum::GetDisplayValueAsText(ActiveAbility).ToString(),
		HealthPercent * 100.f,
		CurrentEnergy, MaxEnergy);

	FColor TextColor = FColor::Green;
	if (HealthPercent <= 0.3f)      TextColor = FColor::Red;
	else if (HealthPercent <= 0.6f) TextColor = FColor::Yellow;

	DrawDebugString(GetWorld(), FVector(0.f, 0.f, DebugTextHeightOffset),
		Text, Owner, TextColor, 0.f, true, DebugTextScale);
#endif
}

// ============================================================
// SELECTINSTINCT — full rewrite matching simulator v8
// ============================================================

void UDragonAIBehaviourComponent::SelectInstinct()
{
	// --- Absolute priorities ---

	if (GetDistanceFromHome() > TerritoryRadius)
	{
		CurrentInstinct = EDragonInstinct::ReturningHome;
		CurrentState = EDragonState::Roaming;
		return;
	}

	/*if (!TargetActor && LastKnownTargetLocation != FVector::ZeroVector)
	{
		CurrentInstinct = EDragonInstinct::Searching;
		CurrentState = EDragonState::Observe;
		return;
	}*/

	// --- Personality shortcuts ---
	float Agg = PersonalitySettings.Aggression;
	float Int = PersonalitySettings.Intelligence;
	float Cour = PersonalitySettings.Courage;
	float Cur = PersonalitySettings.Curiosity;
	float Pat = PersonalitySettings.Patience;
	float Ter = PersonalitySettings.Territoriality;

	float Distance = GetDistanceToTarget();
	bool bInTer = IsInsideTerritory();

	UE_LOG(LogTemp, Warning, TEXT("Distance to target: %.0f (Zone: %s)"),
	Distance,
	Distance > 9000.f ? TEXT("FAR") : Distance > 4000.f ? TEXT("MID") : TEXT("CLOSE"));
	// Courage base modifier (always active)
	// Courage 0 = 0.6, Courage 0.5 = 1.0, Courage 1.0 = 1.4
	float CourAtk = 0.6f + Cour * 0.8f;

	float AttackWeight = 0.f;
	float ThreatWeight = 0.f;
	float StrafeWeight = 0.f;
	float StalkWeight = 0.f;
	float RestWeight = 0.f;
	float RoamingWeight = 0.f;

	if (!TargetActor)
	{
		RoamingWeight = 1.0f;
	}
	else
	{
		// --- ZONE 1: FAR (> 9000) ---
		if (Distance > 9000.f)
		{
			StalkWeight = 5.0f + Cur * 2.0f;
			AttackWeight = Agg * 0.1f;
			// Threatening = 0 (too far)
			// Strafing = 0 (too far)
		}
		// --- ZONE 2: MID (4000 to 9000) ---
		else if (Distance > 4000.f)
		{
			float TerMul = bInTer ? 2.5f : 1.0f;

			AttackWeight = (Agg * 4.0f + 1.0f) * CourAtk;
			ThreatWeight = Agg * 2.0f + Ter * TerMul + Int * 1.5f + 0.5f;
			StalkWeight = Cur * 3.0f + Pat * 1.0f;
			StrafeWeight = Int * 3.0f + Pat * 2.0f + (1.0f - Cour) * 1.5f;
		}
		// --- ZONE 3: CLOSE (< 4000) ---
		else
		{
			AttackWeight = 3.0f + Agg * 3.0f;
			if (bInTer) AttackWeight += Ter * 1.5f;

			// Intelligence suppresses attack at close range
			float IntSup = 1.0f - Int * 0.3f;
			AttackWeight *= IntSup;
			AttackWeight *= CourAtk;

			ThreatWeight = Agg * 1.5f + Int * 1.5f + Ter * (bInTer ? 1.5f : 0.3f);
			StalkWeight = Cur * 1.0f;
			StrafeWeight = Int * 2.5f + (1.0f - Agg) * 2.0f + Pat * 1.5f + (1.0f - Cour) * 1.5f;
		}
	}

	// --- SMOOTH DANGER CURVE (health modifier) ---
	float EffDanger = GetEffectiveDanger();

	if (EffDanger > 0.f)
	{
		// Attack penalty: x1.0 at danger 0, x0.3 at danger 1.0
		AttackWeight *= (1.0f - EffDanger * 0.7f);

		// Strafe boost: +0 at danger 0, +4.0 at danger 1.0
		StrafeWeight += EffDanger * 4.0f;

		// Threatening suppression: wounded dragons don't intimidate well
		ThreatWeight *= (1.0f - EffDanger * 0.4f);
	}

	// Brave rage: high courage + significant danger = attack boost
	if (EffDanger > 0.2f && Cour > 0.6f)
	{
		float RageMul = 1.0f + Cour * 0.5f;
		AttackWeight *= RageMul;
	}

	// --- ENERGY MODIFIERS ---
	EDragonEnergyZone EnergyZone = GetEnergyZone();

	switch (EnergyZone)
	{
	case EDragonEnergyZone::Critical:
		AttackWeight *= 0.3f;
		RestWeight += Pat * 5.0f;
		StrafeWeight += Pat * 1.0f;
		break;

	case EDragonEnergyZone::Low:
		AttackWeight *= 0.6f;
		StrafeWeight += Pat * 1.5f;
		RestWeight += Pat * 3.0f;
		break;

	case EDragonEnergyZone::Full:
		AttackWeight *= 1.3f;
		break;

	default:
		break;
	}

	// --- TERRITORIALITY MODIFIERS ---
	if (!bInTer && Ter > 0.5f)
	{
		// Outside territory: reduce attack motivation
		AttackWeight *= (1.0f - Ter * 0.5f);
	}

	if (bInTer && Ter > 0.4f)
	{
		// Inside territory: suppress strafing (stand your ground)
		StrafeWeight *= (1.0f - Ter * 0.4f);
	}

	// --- INSTINCT WHITELIST ---
	if (AllowedInstincts.Num() > 0)
	{
		if (!IsInstinctAllowed(EDragonInstinct::Attacking))   AttackWeight = 0.f;
		if (!IsInstinctAllowed(EDragonInstinct::Threatening)) ThreatWeight = 0.f;
		if (!IsInstinctAllowed(EDragonInstinct::Strafing))    StrafeWeight = 0.f;
		if (!IsInstinctAllowed(EDragonInstinct::Stalking))    StalkWeight = 0.f;
		if (!IsInstinctAllowed(EDragonInstinct::Resting))     RestWeight = 0.f;
		if (!IsInstinctAllowed(EDragonInstinct::Roaming))     RoamingWeight = 0.f;
	}

	// --- WEIGHTED SELECTION ---
	AttackWeight = FMath::Max(0.f, AttackWeight);
	ThreatWeight = FMath::Max(0.f, ThreatWeight);
	StrafeWeight = FMath::Max(0.f, StrafeWeight);
	StalkWeight = FMath::Max(0.f, StalkWeight);
	RestWeight = FMath::Max(0.f, RestWeight);
	RoamingWeight = FMath::Max(0.f, RoamingWeight);

	float TotalWeight = AttackWeight + ThreatWeight + StrafeWeight +
		StalkWeight + RestWeight + RoamingWeight;

	if (TotalWeight <= 0.f)
	{
		CurrentInstinct = EDragonInstinct::Roaming;
		CurrentState = EDragonState::Roaming;
		return;
	}

	float RandomRoll = FMath::FRandRange(0.f, TotalWeight);
	UE_LOG(LogTemp, Warning,
	TEXT("Roll: %.2f / %.1f  (Atk<%.1f Thr<%.1f Str<%.1f Stk<%.1f)"),
	RandomRoll, TotalWeight,
	AttackWeight,
	AttackWeight + ThreatWeight,
	AttackWeight + ThreatWeight + StrafeWeight,
	AttackWeight + ThreatWeight + StrafeWeight + StalkWeight);

	float Accumulator = 0.f;

	Accumulator += AttackWeight;
	if (RandomRoll < Accumulator) { CurrentInstinct = EDragonInstinct::Attacking; }
	else
	{
		Accumulator += ThreatWeight;
		if (RandomRoll < Accumulator) { CurrentInstinct = EDragonInstinct::Threatening; }
		else
		{
			Accumulator += StrafeWeight;
			if (RandomRoll < Accumulator) { CurrentInstinct = EDragonInstinct::Strafing; }
			else
			{
				Accumulator += StalkWeight;
				if (RandomRoll < Accumulator) { CurrentInstinct = EDragonInstinct::Stalking; }
				else
				{
					Accumulator += RestWeight;
					if (RandomRoll < Accumulator) { CurrentInstinct = EDragonInstinct::Resting; }
					else { CurrentInstinct = EDragonInstinct::Roaming; }
				}
			}
		}
	}

	UE_LOG(LogTemp, Warning,
		TEXT("Instinct: %s (Atk=%.1f Thr=%.1f Str=%.1f Stk=%.1f Rst=%.1f Roam=%.1f)"),
		*UEnum::GetValueAsString(CurrentInstinct),
		AttackWeight, ThreatWeight, StrafeWeight, StalkWeight, RestWeight, RoamingWeight);

	// Update state
	switch (CurrentInstinct)
	{
	case EDragonInstinct::Roaming:
	case EDragonInstinct::ReturningHome:
		CurrentState = EDragonState::Roaming;
		break;
	case EDragonInstinct::Stalking:
	case EDragonInstinct::Threatening:
	case EDragonInstinct::Searching:
		CurrentState = EDragonState::Observe;
		break;
	case EDragonInstinct::Attacking:
		CurrentState = EDragonState::Attack;
		break;
	case EDragonInstinct::Strafing:
		CurrentState = EDragonState::Strafe;
		break;
	case EDragonInstinct::Resting:
		CurrentState = EDragonState::Rest;
		break;
	}
}

// ============================================================
// SELECTABILITY — energy-aware with priority filtering
// ============================================================

void UDragonAIBehaviourComponent::SelectAbility()
{
	UE_LOG(LogTemp, Log, TEXT("SelectAbility"));

	// Passive instincts don't need ability selection
	if (CurrentInstinct == EDragonInstinct::ReturningHome ||
		CurrentInstinct == EDragonInstinct::Roaming)
	{
		CurrentAbility = EDragonAbilityType::None;
		UpdateBlackboard();
		return;
	}

	if (CurrentInstinct == EDragonInstinct::Resting)
	{
		CurrentAbility = EDragonAbilityType::ThermalGlide;
		UpdateBlackboard();
		return;
	}

	if (!TargetActor) return;
	if (!CanPerformAttack()) return;

	float DistanceToTarget = GetDistanceToTarget();
	float AltitudeDiff = GetTargetAltitudeDifference();
	EDragonEnergyZone EnergyZone = GetEnergyZone();

	// Check for recent damage (for Reactive ability selection)
	bool bRecentlyDamaged = bHasPendingStimulus &&
		(PendingStimulus.Type == EDragonStimulus::HeavyDamageTaken ||
		 PendingStimulus.Type == EDragonStimulus::DamageTaken);

	// Health for Desperate ability eligibility
	UDragonStimulusComponent* StimulusComp =
		GetOwner()->FindComponentByClass<UDragonStimulusComponent>();
	float HealthPercent = StimulusComp ? StimulusComp->HealthPercent : 1.f;

	float BestScore = -1.f;
	const FDragonAbilityData* BestAbility = nullptr;

	for (const FDragonAbilityData& Ability : AbilitySettings)
	{
		if (Ability.AbilityType == EDragonAbilityType::None) continue;
		if (Ability.InstinctType != CurrentInstinct) continue;
		if (!CanUseAbility(Ability)) continue;

		// Distance & altitude filters
		if (DistanceToTarget < Ability.PreferredDistanceMin ||
			DistanceToTarget > Ability.PreferredDistanceMax)
			continue;

		if (AltitudeDiff < Ability.MinAltitudeDifference ||
			AltitudeDiff > Ability.MaxAltitudeDifference)
			continue;

		// Priority filtering
		if (Ability.Priority == EDragonAbilityPriority::Reactive && !bRecentlyDamaged)
			continue;
		if (Ability.Priority == EDragonAbilityPriority::Desperate && HealthPercent > 0.25f)
			continue;

		// Scoring
		float RandomFactor = FMath::FRandRange(0.8f, 1.2f);
		float Score = Ability.Weight * RandomFactor;

		// Repeat penalty
		if (Ability.AbilityType == LastUsedAbility) Score *= 0.7f;
		if (Ability.AbilityType == SecondLastUsedAbility) Score *= 0.85f;

		// Energy modifier
		Score *= GetEnergyScoreModifier(Ability);

		// Priority bonuses
		if (bRecentlyDamaged && Ability.Priority == EDragonAbilityPriority::Reactive)
			Score *= 2.5f;

		if (Ability.Priority == EDragonAbilityPriority::Efficient &&
			(EnergyZone == EDragonEnergyZone::Low || EnergyZone == EDragonEnergyZone::Critical))
			Score *= 1.8f;

		if (Ability.Priority == EDragonAbilityPriority::Desperate && HealthPercent < 0.2f)
			Score *= 3.0f;

		// Intelligence prefers efficient
		if (PersonalitySettings.Intelligence > 0.6f)
		{
			float Efficiency = Ability.GetEnergyEfficiency(MaxEnergy);
			Score *= (0.7f + Efficiency * 0.6f);
		}

		if (Score > BestScore)
		{
			BestScore = Score;
			BestAbility = &Ability;
		}
	}

	// Clear pending stimulus
	bHasPendingStimulus = false;

	if (BestAbility)
	{
		AbilityRetryCount = 0;
		UE_LOG(LogTemp, Warning, TEXT("Selected Ability: %s (score=%.2f)"),
			*UEnum::GetValueAsString(BestAbility->AbilityType), BestScore);

		UseAbility(*BestAbility);
		UpdateBlackboard();
	}
	else
	{
		AbilityRetryCount++;

		if (AbilityRetryCount >= MaxAbilityRetries)
		{
			UE_LOG(LogTemp, Warning,
				TEXT("SelectAbility: %d retries failed — forcing Roaming"),
				AbilityRetryCount);

			AbilityRetryCount = 0;
			CurrentInstinct = EDragonInstinct::Roaming;
			CurrentState = EDragonState::Roaming;
			CurrentAbility = EDragonAbilityType::FlyRandom;
			UpdateBlackboard();
			return;
		}

		UE_LOG(LogTemp, Warning,
			TEXT("SelectAbility: no match for [%s] — retry %d/%d"),
			*UEnum::GetValueAsString(CurrentInstinct),
			AbilityRetryCount, MaxAbilityRetries);

		GetWorld()->GetTimerManager().SetTimer(
			AbilityRetryHandle,
			[this]()
			{
				SelectInstinct();
				SelectAbility();
			},
			AbilityRetryInterval,
			false);
	}
}

// ============================================================
// STIMULUS HANDLING
// ============================================================

void UDragonAIBehaviourComponent::OnStimulusReceived(const FDragonStimulusInfo& Stimulus)
{
	if (bReactingStimulusThisFrame) return;
	bReactingStimulusThisFrame = true;

	UDragonAbilityComponent* AbilityComp =
		GetOwner()->FindComponentByClass<UDragonAbilityComponent>();

	switch (Stimulus.Type)
	{
	case EDragonStimulus::DamageTaken:
		{
			if (AbilityComp)
			{
				UE_LOG(LogTemp, Warning, TEXT("Dragon DamageTaken — trying interrupt"));

				bool bInterrupted = AbilityComp->RequestInterrupt(
					EDragonInterruptReason::DamageFlinch);

				if (bInterrupted)
				{
					bInReactionPhase = true;
					LastInterruptReason = EDragonInterruptReason::DamageFlinch;
					return;
				}
				else
				{
					// Only store as pending if interrupt was denied
					PendingStimulus = Stimulus;
					bHasPendingStimulus = true;
				}
			}
			break;
		}
	case EDragonStimulus::HeavyDamageTaken:
	{
		// Flinch check using smooth danger + courage
		float EffDanger = GetEffectiveDanger();

		// Flinch if danger is high enough (brave dragons have lower effective danger)
		if (EffDanger > 0.3f && AbilityComp)
		{
			UE_LOG(LogTemp, Warning,
				TEXT("Dragon flinched! EffDanger=%.2f"),
				EffDanger);

			bool bInterrupted = AbilityComp->RequestInterrupt(
				EDragonInterruptReason::DamageFlinch);

			if (bInterrupted)
			{
				bInReactionPhase = true;
				LastInterruptReason = EDragonInterruptReason::DamageFlinch;
				return;
			}
			else
			{
				PendingStimulus = Stimulus;
				bHasPendingStimulus = true;
			}
		}
		break;
	}

	case EDragonStimulus::HealthCritical:
	{
		UE_LOG(LogTemp, Warning,
			TEXT("Dragon health CRITICAL (%.0f%%)"),
			Stimulus.Value * 100.f);

		if (AbilityComp)
		{
			AbilityComp->RequestInterrupt(EDragonInterruptReason::HealthFlee);
		}

		bInReactionPhase = true;
		LastInterruptReason = EDragonInterruptReason::HealthFlee;
		break;
	}

	case EDragonStimulus::HealthLow:
	{
		// Use smooth danger — if effective danger is high, retreat
		float EffDanger = GetEffectiveDanger();

		if (EffDanger > 0.5f)
		{
			UE_LOG(LogTemp, Warning,
				TEXT("Dragon health low, danger=%.2f — retreating"),
				EffDanger);

			if (AbilityComp)
			{
				AbilityComp->RequestInterrupt(EDragonInterruptReason::HealthFlee);
			}

			bInReactionPhase = true;
			LastInterruptReason = EDragonInterruptReason::HealthFlee;
		}
		break;
	}

	case EDragonStimulus::EnergyCritical:
	{
		UE_LOG(LogTemp, Warning, TEXT("Dragon energy CRITICAL"));

		if (AbilityComp && bAbilityRunning)
		{
			AbilityComp->RequestInterrupt(EDragonInterruptReason::EnergyDepleted);
		}

		if (PersonalitySettings.Patience > PersonalitySettings.Aggression &&
			IsInstinctAllowed(EDragonInstinct::Resting))
		{
			CurrentInstinct = EDragonInstinct::Resting;
			CurrentState = EDragonState::Rest;
		}
		UpdateBlackboard();
		break;
	}

	case EDragonStimulus::EnergyLow:
	{
		float EnergyPercent = MaxEnergy > 0.f ? CurrentEnergy / MaxEnergy : 1.f;
		// Patient dragons rest, aggressive dragons keep going (scoring handles it)
		float RestThreshold = FMath::Lerp(0.15f, 0.40f, PersonalitySettings.Patience);

		if (EnergyPercent <= RestThreshold &&
			IsInstinctAllowed(EDragonInstinct::Resting) &&
			!(PersonalitySettings.Aggression > 0.7f && PersonalitySettings.Intelligence < 0.4f))
		{
			if (AbilityComp && bAbilityRunning)
			{
				AbilityComp->RequestInterrupt(EDragonInterruptReason::EnergyDepleted);
			}

			bInReactionPhase = true;
			LastInterruptReason = EDragonInterruptReason::EnergyDepleted;
			CurrentInstinct = EDragonInstinct::Resting;
			CurrentState = EDragonState::Rest;
			UpdateBlackboard();
		}
		break;
	}

	case EDragonStimulus::EnergyRecovered:
	{
		UE_LOG(LogTemp, Log, TEXT("Energy recovered — re-evaluating"));
		if (CurrentInstinct == EDragonInstinct::Resting)
		{
			SelectInstinct();
			SelectAbility();
		}
		break;
	}

	default:
		break;
	}
}

// ============================================================
// REACTION PHASE
// ============================================================

void UDragonAIBehaviourComponent::SelectReactionAbility()
{
	UDragonStimulusComponent* StimulusComp =
		GetOwner()->FindComponentByClass<UDragonStimulusComponent>();

	float HealthPercent = StimulusComp ? StimulusComp->HealthPercent : 1.f;
	float EnergyPercent = MaxEnergy > 0.f ? CurrentEnergy / MaxEnergy : 1.f;

	EDragonAbilityType ReactionAbility = EDragonAbilityType::None;

	if (HealthPercent < 0.15f)
	{
		// Critical — full retreat
		ReactionAbility = EDragonAbilityType::FlyRandom;
		CurrentInstinct = EDragonInstinct::ReturningHome;
		CurrentState = EDragonState::Flee;
	}
	else if (GetEffectiveDanger() > 0.5f || EnergyPercent < 0.3f)
	{
		// Hurting or low energy — gain altitude
		ReactionAbility = EDragonAbilityType::SkyCircle;
		CurrentInstinct = EDragonInstinct::Stalking;
		CurrentState = EDragonState::Observe;
	}
	else
	{
		// Quick strafe reposition
		float AltDiff = GetTargetAltitudeDifference();
		ReactionAbility = (AltDiff > 500.f)
			? EDragonAbilityType::AirStrafe
			: EDragonAbilityType::GroundStrafe;

		CurrentInstinct = EDragonInstinct::Strafing;
		CurrentState = EDragonState::Strafe;
	}

	// Whitelisted dragons skip reactions outside their allowed instincts
	if (!IsInstinctAllowed(CurrentInstinct))
	{
		SelectInstinct();
		SelectAbility();
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Reaction: %s (HP=%.0f%% Energy=%.0f%%)"),
		*UEnum::GetValueAsString(ReactionAbility),
		HealthPercent * 100.f, EnergyPercent * 100.f);

	const FDragonAbilityData* AbilityData = FindAbilityData(ReactionAbility);

	if (AbilityData && CanUseAbility(*AbilityData))
	{
		UseAbility(*AbilityData);
		UpdateBlackboard();
	}
	else
	{
		UE_LOG(LogTemp, Warning,
			TEXT("Reaction: %s unavailable — normal selection"),
			*UEnum::GetValueAsString(ReactionAbility));
		SelectInstinct();
		SelectAbility();
	}
}

// ============================================================
// ONABILITYFINISHED
// ============================================================

void UDragonAIBehaviourComponent::OnAbilityFinished()
{
	bAbilityRunning = false;
	AbilityRetryCount = 0;
	GetWorld()->GetTimerManager().ClearTimer(AbilityRetryHandle);

	// Priority 1: pending stimulus from during uninterruptible ability
	if (bHasPendingStimulus)
	{
		UE_LOG(LogTemp, Log, TEXT("Processing pending stimulus [%s]"),
			*UEnum::GetValueAsString(PendingStimulus.Type));
		OnStimulusReceived(PendingStimulus);
		bHasPendingStimulus = false;
		return;
	}

	// Priority 2: reaction phase (one reposition then back to normal)
	if (bInReactionPhase)
	{
		bInReactionPhase = false;
		SelectReactionAbility();
		return;
	}

	// Priority 3: normal flow
	SelectInstinct();
	SelectAbility();
}

// ============================================================
// DANGER / ENERGY HELPERS
// ============================================================

float UDragonAIBehaviourComponent::GetDangerLevel() const
{
	UDragonStimulusComponent* StimulusComp =
		GetOwner()->FindComponentByClass<UDragonStimulusComponent>();
	if (!StimulusComp) return 0.f;

	return FMath::Clamp((0.6f - StimulusComp->HealthPercent) / 0.6f, 0.f, 1.f);
}

float UDragonAIBehaviourComponent::GetEffectiveDanger() const
{
	float RawDanger = GetDangerLevel();
	return RawDanger * (1.0f - PersonalitySettings.Courage * 0.6f);
}

EDragonEnergyZone UDragonAIBehaviourComponent::GetEnergyZone() const
{
	if (MaxEnergy <= 0.f) return EDragonEnergyZone::Full;
	float Percent = CurrentEnergy / MaxEnergy;

	if (Percent <= 0.2f) return EDragonEnergyZone::Critical;
	if (Percent <= 0.5f) return EDragonEnergyZone::Low;
	if (Percent <= 0.75f) return EDragonEnergyZone::Normal;
	return EDragonEnergyZone::Full;
}

float UDragonAIBehaviourComponent::GetEnergyScoreModifier(const FDragonAbilityData& AbilityData) const
{
	EDragonEnergyZone Zone = GetEnergyZone();

	switch (Zone)
	{
	case EDragonEnergyZone::Critical:
		return AbilityData.GetEnergyEfficiency(MaxEnergy) * 2.0f;
	case EDragonEnergyZone::Low:
		return 0.5f + AbilityData.GetEnergyEfficiency(MaxEnergy) * 0.8f;
	case EDragonEnergyZone::Normal:
		return 1.0f;
	case EDragonEnergyZone::Full:
		return AbilityData.GetSpendBonus(MaxEnergy);
	}
	return 1.0f;
}

bool UDragonAIBehaviourComponent::IsInsideTerritory() const
{
	return GetDistanceFromHome() <= TerritoryRadius;
}

bool UDragonAIBehaviourComponent::IsInstinctAllowed(EDragonInstinct Instinct) const
{
	return AllowedInstincts.Num() == 0 || AllowedInstincts.Contains(Instinct);
}

// ============================================================
// EXISTING — preserved
// ============================================================

bool UDragonAIBehaviourComponent::CanUseAbility(const FDragonAbilityData& AbilityData) const
{
	if (CurrentEnergy < AbilityData.EnergyCost) return false;
	if (AbilityData.AbilityType == EDragonAbilityType::None) return false;

	const float* LastTime = LastAbilityUseTime.Find(AbilityData.AbilityType);
	if (LastTime)
	{
		float CurrentTime = GetWorld()->GetTimeSeconds();
		if (CurrentTime - *LastTime < AbilityData.Cooldown) return false;
	}
	return true;
}

void UDragonAIBehaviourComponent::ConsumeEnergy(float Amount)
{
	CurrentEnergy -= Amount;
	CurrentEnergy = FMath::Clamp(CurrentEnergy, 0.f, MaxEnergy);
}

void UDragonAIBehaviourComponent::UseAbility(const FDragonAbilityData& AbilityData)
{
	float CurrentTime = GetWorld()->GetTimeSeconds();

	LastAbilityUseTime.Add(AbilityData.AbilityType, CurrentTime);
	ConsumeEnergy(AbilityData.EnergyCost);

	SecondLastUsedAbility = LastUsedAbility;
	LastUsedAbility = AbilityData.AbilityType;
	CurrentAbility = AbilityData.AbilityType;

	float Delay = FMath::FRandRange(MinAttackInterval, MaxAttackInterval);
	NextAttackTime = CurrentTime + Delay;
}

void UDragonAIBehaviourComponent::EvaluateSituation()
{
	if (!GetWorld()) return;

	float CurrentTime = GetWorld()->GetTimeSeconds();

	if (GetDistanceFromHome() > MaxChaseDistance)
	{
		TargetActor = nullptr;
	}

	if (TargetActor)
	{
		LastKnownTargetLocation = TargetActor->GetActorLocation();
		LastSeenTime = CurrentTime;
	}
	else
	{
		if (CurrentTime - LastSeenTime > TargetMemoryDuration)
		{
			LastKnownTargetLocation = FVector::ZeroVector;
		}
	}

	if (CurrentTime >= NextInstinctChangeTime)
	{
		SelectInstinct();
		float Duration = FMath::FRandRange(MinInstinctDuration, MaxInstinctDuration);
		NextInstinctChangeTime = CurrentTime + Duration;
	}

	UpdateBlackboard();
}

float UDragonAIBehaviourComponent::GetDistanceToTarget() const
{
	if (!TargetActor) return -1.f;
	return FVector::Dist(GetOwner()->GetActorLocation(), TargetActor->GetActorLocation());
}

void UDragonAIBehaviourComponent::UpdateBlackboard()
{
	if (!BlackboardComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("BlackboardComponent lost"));
		return;
	}

	BlackboardComponent->SetValueAsEnum("CurrentState", (uint8)CurrentState);
	BlackboardComponent->SetValueAsEnum("CurrentInstinct", (uint8)CurrentInstinct);
	BlackboardComponent->SetValueAsEnum("CurrentAbility", (uint8)LastUsedAbility);
	BlackboardComponent->SetValueAsObject("TargetActor", TargetActor);

	if (TargetActor)
	{
		BlackboardComponent->SetValueAsVector("TargetLocation",
			TargetActor->GetActorLocation());
	}

	BlackboardComponent->SetValueAsVector(
		TEXT("LastKnownTargetLocation"), LastKnownTargetLocation);
}

bool UDragonAIBehaviourComponent::CanPerformAttack() const
{
	if (!GetWorld()) return false;
	return GetWorld()->GetTimeSeconds() >= NextAttackTime;
}

void UDragonAIBehaviourComponent::SetTargetActor(AActor* NewTarget)
{
	TargetActor = NewTarget;

	if (NewTarget)
	{
	//	UE_LOG(LogTemp, Warning, TEXT("Dragon detected target: %s"), *NewTarget->GetName());
		LastKnownTargetLocation = NewTarget->GetActorLocation();
		LastSeenTime = GetWorld()->GetTimeSeconds();

		if (!bAbilityRunning)
		{
			AbilityRetryCount = 0;
			//SelectInstinct();
			//SelectAbility();
		}
	}
	else
	{
	//	UE_LOG(LogTemp, Warning, TEXT("Dragon lost target"));
	}
}

bool UDragonAIBehaviourComponent::IsValidTarget(AActor* Actor) const
{
	if (!Actor) return false;
	if (Actor == GetOwner()) return false;
	if (Actor->IsActorBeingDestroyed()) return false;
	return true;
}

float UDragonAIBehaviourComponent::GetTargetAltitudeDifference() const
{
	if (!TargetActor) return 0.f;
	return GetOwner()->GetActorLocation().Z - TargetActor->GetActorLocation().Z;
}

FVector UDragonAIBehaviourComponent::PredictTargetLocation(float PredictionTime) const
{
	if (!TargetActor) return FVector::ZeroVector;

	FVector TargetLocation = TargetActor->GetActorLocation();
	APawn* TargetPawn = Cast<APawn>(TargetActor);
	if (!TargetPawn) return TargetLocation;

	return TargetLocation + TargetPawn->GetVelocity() * PredictionTime;
}

float UDragonAIBehaviourComponent::GetDistanceFromHome() const
{
	return FVector::Dist(GetOwner()->GetActorLocation(), HomeLocation);
}

void UDragonAIBehaviourComponent::OnAbilityStarted()
{
	bAbilityRunning = true;
}

void UDragonAIBehaviourComponent::SetBlackboard(UBlackboardComponent* InBlackboard)
{
	BlackboardComponent = InBlackboard;
}

const FDragonAbilityData* UDragonAIBehaviourComponent::FindAbilityData(
	EDragonAbilityType InAbilityType) const
{
	for (const FDragonAbilityData& Data : AbilitySettings)
	{
		if (Data.AbilityType == InAbilityType)
		{
			return &Data;
		}
	}
	return nullptr;
}