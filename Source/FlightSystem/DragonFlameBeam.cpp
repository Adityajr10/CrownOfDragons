// Fill out your copyright notice in the Description page of Project Settings.


#include "DragonFlameBeam.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "GameFramework/ProjectileMovementComponent.h"

ADragonFlameBeam::ADragonFlameBeam()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	Projectile = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile"));
	Projectile->InitialSpeed = 4500.f;
	Projectile->MaxSpeed = 4500.f;
	Projectile->ProjectileGravityScale = 0.f;
	Projectile->bRotationFollowsVelocity = true;
}

void ADragonFlameBeam::Init(FVector Start, FVector Dir)
{
	SetActorLocation(Start);
	SetActorRotation(Dir.Rotation());

	Projectile->Velocity = Dir * Projectile->InitialSpeed;

	FX = UNiagaraFunctionLibrary::SpawnSystemAttached(
		FireFX,
		Root,
		NAME_None,
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		EAttachLocation::SnapToTarget,
		true);

	SetLifeSpan(1.2f);
}