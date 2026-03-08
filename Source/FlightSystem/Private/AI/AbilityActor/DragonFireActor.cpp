// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AbilityActor/DragonFireActor.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

ADragonFireActor::ADragonFireActor()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	NiagaraComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("FireFX"));
	NiagaraComp->SetupAttachment(Root);

	SetLifeSpan(5.f);
}

void ADragonFireActor::BeginPlay()
{
	Super::BeginPlay();

	if (FireFX)
	{
		NiagaraComp->SetAsset(FireFX);
	}
}

void ADragonFireActor::Init(FVector Direction)
{
	MoveDir = Direction.GetSafeNormal();
}

void ADragonFireActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetActorLocation(GetActorLocation() + MoveDir * Speed * DeltaTime);
}
