// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AbilityActor/DragonShockwaveActor.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

ADragonShockwaveActor::ADragonShockwaveActor()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	ShockwaveFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ShockwaveFX"));
	ShockwaveFX->SetupAttachment(Root);

	SetLifeSpan(5.f);
}

void ADragonShockwaveActor::BeginPlay()
{
	Super::BeginPlay();

	if (ShockwaveSystem)
	{
		ShockwaveFX->SetAsset(ShockwaveSystem);
	}

	if (ImpactCameraShake)
	{
		UGameplayStatics::PlayWorldCameraShake(
			this, ImpactCameraShake, GetActorLocation(), 0.f, MaxRadius * 1.5f);
	}
}

void ADragonShockwaveActor::InitTravel(const FVector& InTarget)
{
	TravelTarget = InTarget;
	bTravel = true;
}

void ADragonShockwaveActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bExpansionDone) return;

	if (bTravel)
	{
		FVector Dir = (TravelTarget - GetActorLocation()).GetSafeNormal();
		SetActorLocation(GetActorLocation() + Dir * TravelSpeed * DeltaTime);
	}

	CurrentRadius += ExpandSpeed * DeltaTime;

	if (!bPlayerHit)
	{
		ACharacter* Player = Cast<ACharacter>(
			UGameplayStatics::GetPlayerPawn(this, 0));

		if (Player)
		{
			float Dist = FVector::Dist(
				Player->GetActorLocation(), GetActorLocation());

			// Ring caught up with the player
			if (Dist <= CurrentRadius && Dist <= MaxRadius)
			{
				bPlayerHit = true;

				FVector Dir = (Player->GetActorLocation() - GetActorLocation())
					.GetSafeNormal2D();

				Player->LaunchCharacter(
					Dir * PushForce + FVector(0.f, 0.f, LaunchUpForce),
					true, true);

				if (Damage > 0.f)
				{
					UGameplayStatics::ApplyDamage(
						Player, Damage, nullptr, this, nullptr);
				}
			}
		}
	}

	if (CurrentRadius >= MaxRadius)
	{
		bExpansionDone = true;
	}
}
