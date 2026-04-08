#include "AI/Component/DragonStimulusComponent.h"
#include "AI/Component/DragonAIBehaviourComponent.h"
#include "FlightSystem/DragonBaseAI.h"

UDragonStimulusComponent::UDragonStimulusComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UDragonStimulusComponent::BeginPlay()
{
	Super::BeginPlay();
	OwnerDragon = Cast<ADragonBaseAI>(GetOwner());
}

void UDragonStimulusComponent::TickComponent(
	float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!OwnerDragon) return;

	// Update health percent from DragonBaseAI
	if (OwnerDragon->MaxHealth > 0.f)
	{
		PreviousHealthPercent = HealthPercent;
		HealthPercent = OwnerDragon->CurrentHealth / OwnerDragon->MaxHealth;
	}

	// Monitor energy thresholds
	UDragonAIBehaviourComponent* Behaviour =
		OwnerDragon->FindComponentByClass<UDragonAIBehaviourComponent>();

	if (Behaviour && Behaviour->MaxEnergy > 0.f)
	{
		float EnergyPercent = Behaviour->CurrentEnergy / Behaviour->MaxEnergy;
		CheckEnergyThresholds(EnergyPercent);
	}
}

void UDragonStimulusComponent::ReportDamage(float Amount, AActor* Instigator)
{
	if (!OwnerDragon) return;
	if (Amount <= 0.f) return;

	// Always fire basic DamageTaken
	FireStimulus(EDragonStimulus::DamageTaken, 0.4f, Instigator, Amount);

	// Check for burst damage
	float DamagePercent = PreviousHealthPercent - HealthPercent;
	if (DamagePercent >= BurstDamageThreshold)
	{
		FireStimulus(EDragonStimulus::HeavyDamageTaken, 0.8f, Instigator, DamagePercent);
	}

	// Check health threshold crossings
	if (PreviousHealthPercent > LowHealthThreshold && HealthPercent <= LowHealthThreshold)
	{
		if (!bHealthLowFired)
		{
			bHealthLowFired = true;
			FireStimulus(EDragonStimulus::HealthLow, 0.7f, Instigator, HealthPercent);
		}
	}

	if (PreviousHealthPercent > CriticalHealthThreshold && HealthPercent <= CriticalHealthThreshold)
	{
		if (!bHealthCriticalFired)
		{
			bHealthCriticalFired = true;
			FireStimulus(EDragonStimulus::HealthCritical, 1.0f, Instigator, HealthPercent);
		}
	}
}

void UDragonStimulusComponent::CheckEnergyThresholds(float EnergyPercent)
{
	if (EnergyPercent <= LowEnergyThreshold && !bEnergyLowFired)
	{
		bEnergyLowFired = true;
		bEnergyRecoveredFired = false;
		FireStimulus(EDragonStimulus::EnergyLow, 0.5f, nullptr, EnergyPercent);
	}

	if (EnergyPercent <= CriticalEnergyThreshold && !bEnergyCriticalFired)
	{
		bEnergyCriticalFired = true;
		FireStimulus(EDragonStimulus::EnergyCritical, 0.8f, nullptr, EnergyPercent);
	}

	if (EnergyPercent >= EnergyRecoveredThreshold && !bEnergyRecoveredFired && bEnergyLowFired)
	{
		bEnergyRecoveredFired = true;
		bEnergyLowFired = false;
		bEnergyCriticalFired = false;
		FireStimulus(EDragonStimulus::EnergyRecovered, 0.4f, nullptr, EnergyPercent);
	}
}

void UDragonStimulusComponent::FireStimulus(
	EDragonStimulus Type, float Priority, AActor* Source, float Value)
{
	float CurrentTime = GetWorld()->GetTimeSeconds();
	float* LastTime = LastStimulusTime.Find(Type);

	if (LastTime && (CurrentTime - *LastTime) < StimulusCooldown)
	{
		return;
	}

	LastStimulusTime.Add(Type, CurrentTime);

	FDragonStimulusInfo Info;
	Info.Type = Type;
	Info.Priority = Priority;
	Info.Source = Source;
	Info.Value = Value;

	UE_LOG(LogTemp, Log, TEXT("Stimulus: [%s] priority=%.2f value=%.2f"),
		*UEnum::GetValueAsString(Type), Priority, Value);

	OnStimulusReceived.Broadcast(Info);
}