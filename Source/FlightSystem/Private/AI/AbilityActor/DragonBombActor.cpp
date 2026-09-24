// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AbilityActor/DragonBombActor.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

ADragonBombActor::ADragonBombActor()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	TrailFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("TrailFX"));
	TrailFX->SetupAttachment(Root);

	SetLifeSpan(8.f);
}

void ADragonBombActor::Init(const FVector& InVelocity)
{
	Velocity = InVelocity;
}

void ADragonBombActor::BeginPlay()
{
	Super::BeginPlay();

	if (TrailSystem)
	{
		TrailFX->SetAsset(TrailSystem);
	}
}

void ADragonBombActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bExploded) return;

	Velocity.Z -= Gravity * DeltaTime;

	FVector Start = GetActorLocation();
	FVector End = Start + Velocity * DeltaTime;

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	if (GetOwner()) Params.AddIgnoredActor(GetOwner());

	if (GetWorld()->LineTraceSingleByChannel(
		Hit, Start, End, ECC_Visibility, Params))
	{
		Explode(Hit.ImpactPoint);
		return;
	}

	SetActorLocation(End);
}

void ADragonBombActor::Explode(const FVector& ImpactPoint)
{
	bExploded = true;

	if (ExplosionSystem)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			this, ExplosionSystem, ImpactPoint);
	}

	if (LingerFlameSystem)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			this, LingerFlameSystem, ImpactPoint);
	}

	if (ExplosionCameraShake)
	{
		UGameplayStatics::PlayWorldCameraShake(
			this, ExplosionCameraShake, ImpactPoint, 0.f, ExplosionRadius * 3.f);
	}

	ACharacter* Player = Cast<ACharacter>(
		UGameplayStatics::GetPlayerPawn(this, 0));

	if (Player)
	{
		float Dist = FVector::Dist(Player->GetActorLocation(), ImpactPoint);

		if (Dist < ExplosionRadius)
		{
			FVector Dir = (Player->GetActorLocation() - ImpactPoint)
				.GetSafeNormal();

			Player->LaunchCharacter(
				Dir * PushForce + FVector(0.f, 0.f, 300.f),
				true, true);

			if (Damage > 0.f)
			{
				UGameplayStatics::ApplyDamage(
					Player, Damage, nullptr, this, nullptr);
			}
		}
	}

	Destroy();
}
