// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Component/DragonAIBehaviourComponent.h"

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
}

void UDragonAIBehaviourComponent::TickComponent(
	float DeltaTime,
	ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	EvaluateSituation();
}

bool UDragonAIBehaviourComponent::CanUseAbility(const FDragonAbilityData& AbilityData) const
{
	// Check Energy 
	if (CurrentEnergy < AbilityData.EnergyCost)
	{
		return false;
	}
	//Check Cooldown 
	const float* LastTime = LastAbilityUseTime.Find(AbilityData.AbilityType);

	if (LastTime)
	{
		float CurrentTime = GetWorld()->GetTimeSeconds();

		if (CurrentTime - *LastTime < AbilityData.Cooldown)
		{
			UE_LOG(LogTemp, Warning, TEXT("Ability on cooldown: %s"),
			   *UEnum::GetValueAsString(AbilityData.AbilityType));
			return false;
		}
	}

	return true;
}

void UDragonAIBehaviourComponent::ConsumeEnergy(float Amount)
{
	CurrentEnergy -= Amount;

	CurrentEnergy = FMath::Clamp(CurrentEnergy, 0.f, MaxEnergy);
}

void UDragonAIBehaviourComponent::RegenerateEnergy(float DeltaTime)
{
	CurrentEnergy += EnergyRegenRate * DeltaTime;

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

	//Set next attack time 

	float Delay = FMath::FRandRange(MinAttackInterval, MaxAttackInterval);

	NextAttackTime = CurrentTime + Delay;
}

void UDragonAIBehaviourComponent::EvaluateSituation()
{
	if (!GetWorld())
	{
		return;
	}
	float CurrentTime = GetWorld()->GetTimeSeconds();

	//Territory Check 

	float DistanceFromHome = GetDistanceFromHome();

	if (DistanceFromHome > MaxChaseDistance)
	{
		TargetActor = nullptr;
	}
	//Target Memory Update 
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
	SelectAbility();
	UpdateBlackboard();
}

void UDragonAIBehaviourComponent::SelectInstinct() 
{
    // 1. Early exits (Absolute Priorities)
    if (GetDistanceFromHome() > TerritoryRadius) {
        CurrentInstinct = EDragonInstinct::ReturningHome;
        CurrentState = EDragonState::Roaming;
        return;
    }

    if (!TargetActor && LastKnownTargetLocation != FVector::ZeroVector) {
        CurrentInstinct = EDragonInstinct::Searching;
        CurrentState = EDragonState::Observe;
        return;
    }
	
	float Distance = GetDistanceToTarget();
    UE_LOG(LogTemp, Warning, TEXT("Distance: %.2f (Agg=%.2f, Int=%.2f)"), Distance, PersonalitySettings.Aggression, PersonalitySettings.Intelligence);

    float AttackWeight   = 0.f;
    float ThreatWeight   = 0.f;
    float StalkingWeight = 0.f;
    float RoamingWeight  = 0.f;

    if (!TargetActor)
    {
        RoamingWeight = 1.0f;
    }
    else
    {
        // --- ZONE 1: FAR (> 9000) ---
        if (Distance > 9000.f)
        {
            // Force Stalking. Personality only adds flavor, not logic-breaking aggression.
            StalkingWeight = 5.0f; 
            AttackWeight   = PersonalitySettings.Aggression * 0.1f; // Tiny chance to snap
        }
        // --- ZONE 2: MID (4000 to 9000) ---
        else if (Distance > 4000.f)
        {
            // Personality is the king here.
            // High Aggression = Attack | High Intelligence = Threaten
            AttackWeight = PersonalitySettings.Aggression * 4.0f;
            ThreatWeight = PersonalitySettings.Intelligence * 4.0f;
        	StalkingWeight = PersonalitySettings.Patience * 3.0f;
            
            // Add a base floor so they don't just stand still if stats are 0
            AttackWeight += 0.5f;
            ThreatWeight += 0.5f;
        }
        // --- ZONE 3: CLOSE (< 4000) ---
        else
        {
            // Close range is mostly attacking.
            AttackWeight = 5.0f + (PersonalitySettings.Aggression * 2.0f);
            ThreatWeight = 1.0f + (PersonalitySettings.Intelligence * 2.0f);
        }
    }

    // --- WEIGHTED SELECTION ---
    float TotalWeight = AttackWeight + ThreatWeight + StalkingWeight + RoamingWeight;
    
    // Safety check to avoid divide by zero
    if (TotalWeight <= 0.f) { CurrentInstinct = EDragonInstinct::Roaming; return; }

    float RandomRoll = FMath::FRandRange(0.f, TotalWeight);

    if (RandomRoll < AttackWeight) 
        CurrentInstinct = EDragonInstinct::Attacking;
    else if (RandomRoll < (AttackWeight + ThreatWeight)) 
        CurrentInstinct = EDragonInstinct::Threatening;
    else if (RandomRoll < (AttackWeight + ThreatWeight + StalkingWeight)) 
        CurrentInstinct = EDragonInstinct::Stalking;
    else 
        CurrentInstinct = EDragonInstinct::Roaming;

    // Debug
   FString InstinctName = UEnum::GetValueAsString(CurrentInstinct);
    UE_LOG(LogTemp, Warning, TEXT("Dragon Instinct: %s (Agg=%.2f Int=%.2f)"), *InstinctName,
           PersonalitySettings.Aggression, PersonalitySettings.Intelligence);

    // Update State
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

    case EDragonInstinct::Resting:
        CurrentState = EDragonState::Rest;
        break;
    }
}

/*void UDragonAIBehaviourComponent::SelectInstinct()
{
	if (GetDistanceFromHome() > TerritoryRadius)
	{
		CurrentInstinct = EDragonInstinct::ReturningHome;
		CurrentState = EDragonState::Roaming;
		return;
	}
	//Search behavior if target lost but memory exists
	if (!TargetActor && LastKnownTargetLocation != FVector::ZeroVector)
	{
		CurrentInstinct = EDragonInstinct::Searching;
		CurrentState = EDragonState::Observe;
		return;
	}

	float Distance = GetDistanceToTarget();

	float RoamingScore = 0.f;
	float StalkingScore = 0.f;
	float ThreatScore = 0.f;
	float AttackScore = 0.f;
	float RestScore = 0.f;

	// No target 
	if (!TargetActor)
	{
		RoamingScore += 1.0f;
	}

	else
	{
		// Distance influence 
		if (Distance > 8000.f)
		{
			StalkingScore += 0.8f;
		}
		else if (Distance > 3000.f)
		{
			ThreatScore += 0.7f;
			AttackScore += 0.3f; // allow attacks at medium range
		}
		else
		{
			AttackScore += 0.8f;
		}

		//Personality influence
		AttackScore += PersonalitySettings.Aggression * 0.5f;

		StalkingScore += PersonalitySettings.Patience * 0.3f;

		ThreatScore += PersonalitySettings.Intelligence * 0.2f;
	}

	//Energy influence
	if (CurrentEnergy < 20.f)
	{
		RestScore += 1.0f;
	}
	float RandomMin = 0.8f;
float RandomMax = 1.2f;

RoamingScore  *= FMath::FRandRange(RandomMin, RandomMax);
StalkingScore *= FMath::FRandRange(RandomMin, RandomMax);
ThreatScore   *= FMath::FRandRange(RandomMin, RandomMax);
AttackScore   *= FMath::FRandRange(RandomMin, RandomMax);
RestScore     *= FMath::FRandRange(RandomMin, RandomMax);

	//Find highest score 
	float BestScore = RoamingScore;
	CurrentInstinct = EDragonInstinct::Roaming;

	if (StalkingScore > BestScore)
	{
		BestScore = StalkingScore;
		CurrentInstinct = EDragonInstinct::Stalking;
	}

	if (ThreatScore > BestScore)
	{
		BestScore = ThreatScore;
		CurrentInstinct = EDragonInstinct::Threatening;
	}

	if (AttackScore > BestScore)
	{
		BestScore = AttackScore;
		CurrentInstinct = EDragonInstinct::Attacking;
	}

	if (RestScore > BestScore)
	{
		BestScore = RestScore;
		CurrentInstinct = EDragonInstinct::Resting;
	}
	FString InstinctName = UEnum::GetValueAsString(CurrentInstinct);

	UE_LOG(LogTemp, Warning, TEXT("Dragon Instinct: %s"), *InstinctName);

	// Update State
	switch (CurrentInstinct)
	{
	case EDragonInstinct::Roaming:
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
		
	case EDragonInstinct::ReturningHome:
		CurrentState = EDragonState::Roaming;
		break;

	case EDragonInstinct::Resting:
		CurrentState = EDragonState::Rest;
		break;
	}
}*/

void UDragonAIBehaviourComponent::SelectAbility()
{
	UE_LOG(LogTemp, Warning, TEXT("AbilitySettings Num: %d"), AbilitySettings.Num());
	if (!TargetActor)
	{
		return;
	}

	if (!CanPerformAttack())
	{
		UE_LOG(LogTemp, Warning, TEXT("Your message here"));
		return;
	}
	//UE_LOG(LogTemp, Warning, TEXT("Nomral logs"));
	float DistanceToTarget = GetDistanceToTarget();
	float AltitudeDiff = GetTargetAltitudeDifference();   // ← ADD HERE

	float BestScore = -1.f;
	const FDragonAbilityData* BestAbility = nullptr;

	for (const FDragonAbilityData& Ability : AbilitySettings)
	{
		if (Ability.InstinctType != CurrentInstinct)
		{
			UE_LOG(LogTemp, Warning, TEXT("Not CurrentInstinct"));
			continue;
		}

		if (!CanUseAbility(Ability))
		{
			UE_LOG(LogTemp, Warning, TEXT("!CanUseAbility"));
			continue;
		}

		// Distance check
		if (DistanceToTarget < Ability.PreferredDistanceMin ||
			DistanceToTarget > Ability.PreferredDistanceMax)
		{
			UE_LOG(LogTemp, Warning, TEXT("DistanceToTarget not valid"));
			continue;
		}

		// Altitude check (example rule)
		if (AltitudeDiff < Ability.MinAltitudeDifference ||
	    AltitudeDiff > Ability.MaxAltitudeDifference)
		{
			UE_LOG(LogTemp, Warning, TEXT("AltitudeDiff not valid"));
			continue;
		}

		// Base score
		float RandomFactor = FMath::FRandRange(0.8f, 1.2f);
		float Score = Ability.Weight * RandomFactor;

		// Ability repetition penalty
		if (Ability.AbilityType == LastUsedAbility)
		{
			Score *= 0.7f;
		}

		if (Ability.AbilityType == SecondLastUsedAbility)
		{
			Score *= 0.85f;
		}

		// Select best ability
		if (Score > BestScore)
		{
			BestScore = Score;
			BestAbility = &Ability;
		}
	}
	

	if (BestAbility)
	{
		UE_LOG(LogTemp, Warning, TEXT("Selected Ability: %s"),
		*UEnum::GetValueAsString(BestAbility->AbilityType));
		UseAbility(*BestAbility);
	}
}

float UDragonAIBehaviourComponent::GetDistanceToTarget() const
{
	if (!TargetActor)
	{
		return -1.f;
	}

	return FVector::Dist(
		GetOwner()->GetActorLocation(),
		TargetActor->GetActorLocation()
	);
}

void UDragonAIBehaviourComponent::UpdateBlackboard()
{
	if (!BlackboardComponent)
	{
		return;
	}

	BlackboardComponent->SetValueAsEnum(
		TEXT("CurrentState"),
		static_cast<uint8>(CurrentState)
	);

	BlackboardComponent->SetValueAsEnum(
		TEXT("CurrentAbility"),
		static_cast<uint8>(CurrentAbility)
	);

	BlackboardComponent->SetValueAsObject(
		TEXT("TargetActor"),
		TargetActor
	);
	BlackboardComponent->SetValueAsVector(
	TEXT("LastKnownTargetLocation"),
	LastKnownTargetLocation
);
}

bool UDragonAIBehaviourComponent::CanPerformAttack() const
{
	if (!GetWorld())
	{
		return false;
	}

	float CurrentTime = GetWorld()->GetTimeSeconds();

	return CurrentTime >= NextAttackTime;
}

void UDragonAIBehaviourComponent::SetTargetActor(AActor* NewTarget)
{
	TargetActor = NewTarget;
	if (NewTarget)
	{
		UE_LOG(LogTemp, Warning, TEXT("Dragon detected target: %s"), *NewTarget->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Dragon lost target"));
	}
	if (NewTarget)
	{
		LastKnownTargetLocation = NewTarget->GetActorLocation();
		LastSeenTime = GetWorld()->GetTimeSeconds();
	}
}

bool UDragonAIBehaviourComponent::IsValidTarget(AActor* Actor) const
{
	if (!Actor)
	{
		return false;
	}

	if (Actor == GetOwner())
	{
		return false;
	}

	if (Actor->IsActorBeingDestroyed())
	{
		return false;
	}

	return true;
}

float UDragonAIBehaviourComponent::GetTargetAltitudeDifference() const
{
	if (!TargetActor)
	{
		return 0.f;
	}

	float DragonZ = GetOwner()->GetActorLocation().Z;
	float TargetZ = TargetActor->GetActorLocation().Z;

	return DragonZ - TargetZ;
}

FVector UDragonAIBehaviourComponent::PredictTargetLocation(float PredictionTime) const
{
	if (!TargetActor)
	{
		return FVector::ZeroVector;
	}

	FVector TargetLocation = TargetActor->GetActorLocation();

	APawn* TargetPawn = Cast<APawn>(TargetActor);

	if (!TargetPawn)
	{
		return TargetLocation;
	}

	FVector Velocity = TargetPawn->GetVelocity();

	return TargetLocation + Velocity * PredictionTime;
}

float UDragonAIBehaviourComponent::GetDistanceFromHome() const
{
	return FVector::Dist(GetOwner()->GetActorLocation(), HomeLocation);
}