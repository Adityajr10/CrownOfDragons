#include "DragonBaseAI.h"
#include "DragonFlightComponent.h"
#include "AI/Component/DragonAbilityComponent.h"
#include "AI/Component/DragonStimulusComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

ADragonBaseAI::ADragonBaseAI()
{
	PrimaryActorTick.bCanEverTick = true;

	FlightComponent = CreateDefaultSubobject<UDragonFlightComponent>(TEXT("FlightComponent"));
	FireBreath = CreateDefaultSubobject<UDragonFireBreathComponent>(TEXT("FireBreath"));
	AbilityComponent = CreateDefaultSubobject<UDragonAbilityComponent>(TEXT("AbilityComponent"));

	// NEW: Create stimulus component
	StimulusComponent = CreateDefaultSubobject<UDragonStimulusComponent>(TEXT("StimulusComponent"));

	GetMesh()->SetGenerateOverlapEvents(true);
}

void ADragonBaseAI::BeginPlay()
{
	Super::BeginPlay();
	StartLocation = GetActorLocation();

	CurrentHealth = MaxHealth;

	GetMesh()->OnComponentBeginOverlap.AddDynamic(this, &ADragonBaseAI::OnOverlapBegin);
}

void ADragonBaseAI::TakeoffFromPerch()
{
	IsFlying = true;

	if (TakeoffMontage)
		PlayAnimMontage(TakeoffMontage);

	FVector UpBoost = GetActorUpVector() * 3500.f;
	LaunchCharacter(UpBoost, true, true);
}

void ADragonBaseAI::StartDiveBomb(AActor* Target)
{
	if (!Target) return;

	IsFlying = true;

	if (DiveStartMontage)
		PlayAnimMontage(DiveStartMontage);

	FVector AboveTarget = Target->GetActorLocation();
	AboveTarget.Z += DiveHeight;

	SetActorLocation(AboveTarget);

	GetCharacterMovement()->Velocity = FVector::ZeroVector;

	FVector DiveDir = (Target->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	LaunchCharacter(DiveDir * DiveSpeed, true, true);
}

void ADragonBaseAI::OnDiveImpact()
{
	if (DiveEndMontage)
		PlayAnimMontage(DiveEndMontage);
}

void ADragonBaseAI::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor) return;

	ACharacter* OtherCharacter = Cast<ACharacter>(OtherActor);
	if (!OtherCharacter) return;

	APlayerController* PC = Cast<APlayerController>(OtherCharacter->GetController());
	if (!PC) return;

	float CurrentTime = GetWorld()->GetTimeSeconds();
	if (CurrentTime - LastDamageTime < DamageCooldown) return;

	LastDamageTime = CurrentTime;

	//float DamageAmount = 5.f;
	float DamageAmount = 0.f;
	CurrentHealth -= DamageAmount;
	CurrentHealth = FMath::Clamp(CurrentHealth, 0.f, MaxHealth);

	UE_LOG(LogTemp, Warning, TEXT("Dragon health: %.1f / %.1f"), CurrentHealth, MaxHealth);
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,                      // Key (-1 = always new message)
			2.0f,                    // Time to display (seconds)
			FColor::Green,           // Text color
			FString::Printf(TEXT("Dragon Health: %.1f / %.1f"), CurrentHealth, MaxHealth)
		);
	}
	// NEW: Report damage to stimulus component
	if (StimulusComponent)
	{
		StimulusComponent->ReportDamage(DamageAmount, OtherActor);
	}
}


void ADragonBaseAI::IncreaseHealth()
{
	// Start timer for 20 seconds
	GetWorldTimerManager().SetTimer(
		HealthTimerHandle,
		this,
		&ADragonBaseAI::HealthModification,
		120.0f,
		false
	);
}

void ADragonBaseAI::HealthModification()
{
	// Modify health (example: increase by 20)
	CurrentHealth += 60.0f;

	UE_LOG(LogTemp, Warning, TEXT("Health increased to: %f"), CurrentHealth);
	// Display on screen
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,                     // Key (-1 = new message)
			5.0f,                  // Time in seconds
			FColor::Green,        // Color
			FString::Printf(TEXT("Dragon Health: %.1f"), CurrentHealth)
		);
	}
}