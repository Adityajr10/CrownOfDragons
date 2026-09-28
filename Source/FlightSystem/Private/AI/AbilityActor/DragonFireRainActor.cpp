// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AbilityActor/DragonFireRainActor.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

ADragonFireRainActor::ADragonFireRainActor()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	NiagaraComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("FireFX"));
	NiagaraComp->SetupAttachment(Root);
}

void ADragonFireRainActor::BeginPlay()
{
	Super::BeginPlay();

	if (FireFX)
	{
		NiagaraComp->SetAsset(FireFX);
	}
}

void ADragonFireRainActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector Loc = GetActorLocation();
	Loc.Z -= FallSpeed * DeltaTime;

	SetActorLocation(Loc);

	FHitResult Hit;

	FVector Start = Loc;
	FVector End = Loc - FVector(0,0,100);

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		Hit,
		Start,
		End,
		ECC_Visibility
	);

	if (bHit)
	{
		Explode();
	}
}

void ADragonFireRainActor::Explode()
{
	TArray<AActor*> Ignore;

	UGameplayStatics::ApplyRadialDamage(
		this,
		Damage,
		GetActorLocation(),
		DamageRadius,
		nullptr,
		Ignore,
		this,
		nullptr,
		true
	);

	Destroy();
}