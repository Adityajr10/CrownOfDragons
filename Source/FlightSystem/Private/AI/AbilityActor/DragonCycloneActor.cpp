// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AbilityActor/DragonCycloneActor.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

ADragonCycloneActor::ADragonCycloneActor()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	CycloneFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("CycloneFX"));
	CycloneFX->SetupAttachment(Root);
}

void ADragonCycloneActor::Init(const FVector& InTarget)
{
	TargetLocation = InTarget;
	bHasTarget = true;
}

void ADragonCycloneActor::BeginPlay()
{
	Super::BeginPlay();

	if (CycloneSystem)
	{
		CycloneFX->SetAsset(CycloneSystem);
	}

	SetLifeSpan(LifeDuration + 1.f);
}

void ADragonCycloneActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	LifeTimer += DeltaTime;

	if (LifeTimer >= LifeDuration)
	{
		Destroy();
		return;
	}

	// Slow drift toward the player's current position
	ACharacter* Player = Cast<ACharacter>(
		UGameplayStatics::GetPlayerPawn(this, 0));

	FVector DriftTarget = bHasTarget ? TargetLocation : GetActorLocation();
	if (Player)
	{
		DriftTarget = Player->GetActorLocation();
	}

	FVector Dir = (DriftTarget - GetActorLocation()).GetSafeNormal2D();
	SetActorLocation(GetActorLocation() + Dir * HomingSpeed * DeltaTime);

	PushTimer += DeltaTime;
	if (PushTimer >= PushInterval)
	{
		PushTimer = 0.f;
		ApplyPush();
	}
}

void ADragonCycloneActor::ApplyPush()
{
	ACharacter* Player = Cast<ACharacter>(
		UGameplayStatics::GetPlayerPawn(this, 0));

	if (!Player) return;

	float Dist = FVector::Dist(
		Player->GetActorLocation(), GetActorLocation());

	if (Dist < PushRadius)
	{
		FVector Dir = (Player->GetActorLocation() - GetActorLocation())
			.GetSafeNormal2D();

		Player->LaunchCharacter(
			Dir * PushForce + FVector(0.f, 0.f, 250.f),
			true, true);
	}
}
