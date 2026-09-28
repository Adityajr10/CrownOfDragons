// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AbilityActor/DragonWingGustActor.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

ADragonWingGustActor::ADragonWingGustActor()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	GustFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("GustFX"));
	GustFX->SetupAttachment(Root);
	SetLifeSpan(4.f);
}

void ADragonWingGustActor::Init(const FVector& InTarget)
{
	TargetLocation = InTarget;
}

void ADragonWingGustActor::BeginPlay()
{
	Super::BeginPlay();

	if (GustSystem)
	{
		GustFX->SetAsset(GustSystem);
	}

	ApplyPush();
}
void ADragonWingGustActor::Tick(float DeltaTime)
{
	LifeTimer += DeltaTime;

	if (LifeTimer >= LifeDuration)
	{
		Destroy();
		return;
	}

	FVector Dir = (TargetLocation - GetActorLocation()).GetSafeNormal();

	SetActorLocation(
		GetActorLocation() + Dir * Speed * DeltaTime
	);
}

void ADragonWingGustActor::ApplyPush()
{
	ACharacter* Player = Cast<ACharacter>(
		UGameplayStatics::GetPlayerPawn(this, 0));

	if (!Player) return;

	FVector Dir =
		(Player->GetActorLocation() - GetActorLocation()).GetSafeNormal();

	float Dist =
		FVector::Dist(Player->GetActorLocation(), GetActorLocation());

	if (Dist < GustRadius)
	{
		Player->LaunchCharacter(
			Dir * PushForce + FVector(0,0,300),
			true,
			true
		);
	}
}
