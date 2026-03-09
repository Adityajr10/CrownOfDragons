// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AbilityActor/DragonStrafeFireActor.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

ADragonStrafeFireActor::ADragonStrafeFireActor()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	NiagaraComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("FireFX"));
	NiagaraComp->SetupAttachment(Root);
}

void ADragonStrafeFireActor::BeginPlay()
{
	Super::BeginPlay();

	if (FireFX)
	{
		NiagaraComp->SetAsset(FireFX);
	}

	Player = UGameplayStatics::GetPlayerPawn(this, 0);
}

void ADragonStrafeFireActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!Player) return;

	FVector Loc = GetActorLocation();
	FVector PlayerLoc = Player->GetActorLocation();

	float Dist = FVector::Dist(Loc, PlayerLoc);

	if (Dist < DamageRadius)
	{
		UGameplayStatics::ApplyDamage(
			Player,
			DamagePerSecond * DeltaTime,
			nullptr,
			this,
			nullptr
		);
	}
}

void ADragonStrafeFireActor::StopFire()
{
	if (NiagaraComp)
	{
		NiagaraComp->Deactivate();
	}

	Destroy();
}