// Fill out your copyright notice in the Description page of Project Settings.


#include "DragonFireBreathComponent.h"
#include "DragonFlameBeam.h"
#include "NiagaraFunctionLibrary.h"
#include "GameFramework/Character.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"

UDragonFireBreathComponent::UDragonFireBreathComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}
void UDragonFireBreathComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!ActiveFire || !CurrentTarget) return;

	ACharacter* Dragon = Cast<ACharacter>(GetOwner());
	if (!Dragon) return;

	FVector MouthLoc = Dragon->GetMesh()->GetSocketLocation(MouthSocket);
	FVector Dir = (CurrentTarget->GetActorLocation() - MouthLoc).GetSafeNormal();

	// Force Niagara to aim toward player
	ActiveFire->SetVariableVec3(TEXT("User.FireDirection"), Dir);
}



void UDragonFireBreathComponent::BeginPlay()
{
	Super::BeginPlay();
	ACharacter* Dragon = Cast<ACharacter>(GetOwner());
	if (!Dragon) return;

	AimPivot = NewObject<USceneComponent>(Dragon);
	AimPivot->RegisterComponent();
	AimPivot->AttachToComponent(Dragon->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, MouthSocket);
}

/*
void UDragonFireBreathComponent::StartFire(AActor* Target)
{
	if (bOnCooldown || !FireEffect || !Target) return;

	bOnCooldown = true;
	CurrentTarget = Target;

	EmitFire();   // Single cinematic blast

	GetWorld()->GetTimerManager().SetTimer(
		FireTimer, this,
		&UDragonFireBreathComponent::ResetCooldown,
		FireCooldown, false);
}
*/

void UDragonFireBreathComponent::StartFire(AActor* Target)
{
	if (!bCanFire || !Target || !FireEffect) return;

	bCanFire = false;
	CurrentTarget = Target;

	ACharacter* Dragon = Cast<ACharacter>(GetOwner());
	if (!Dragon) return;

	// Play montage
	if (FireMontage)
		Dragon->PlayAnimMontage(FireMontage);

	USkeletalMeshComponent* Mesh = Dragon->GetMesh();
	
	// Calculate direction to player BEFORE spawning
	FVector MouthLoc = Mesh->GetSocketLocation(MouthSocket);
	FVector TargetLoc = Target->GetActorLocation();
	FVector Direction = (TargetLoc - MouthLoc).GetSafeNormal();
	FRotator FireRotation = Direction.Rotation();

	// Spawn with the correct rotation toward player
	ActiveFire = UNiagaraFunctionLibrary::SpawnSystemAttached(
		FireEffect,
		Mesh,
		MouthSocket,
		FVector::ZeroVector,
		FireRotation,  // Changed from FRotator::ZeroRotator to actual direction
		EAttachLocation::SnapToTarget,
		true);

	// Stop breath after duration
	GetWorld()->GetTimerManager().SetTimer(
		BreathTimer, this,
		&UDragonFireBreathComponent::StopFire,
		FireDuration, false);

	// Enable next fire after cooldown
	GetWorld()->GetTimerManager().SetTimer(
		CooldownTimer, this,
		&UDragonFireBreathComponent::ResetCooldown,
		FireCooldown, false);
}



void UDragonFireBreathComponent::ResetCooldown()
{
	bOnCooldown = false;
	bCanFire = true;
}

/*void UDragonFireBreathComponent::StopFire()
{
	bOnCooldown = false;
	GetWorld()->GetTimerManager().ClearTimer(FireTimer);
}*/
void UDragonFireBreathComponent::StopFire()
{
	if (ActiveFire)
	{
		ActiveFire->DestroyComponent();
		ActiveFire = nullptr;
	}
}

/*void UDragonFireBreathComponent::EmitFire()
{
	UE_LOG(LogTemp, Warning, TEXT("🔥 FIRE EMITTED"));

	ACharacter* Dragon = Cast<ACharacter>(GetOwner());
	if (!Dragon || !CurrentTarget) return;

	FVector Start = Dragon->GetMesh()->GetSocketLocation(MouthSocket);
	FVector End = CurrentTarget->GetActorLocation();

	UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		GetWorld(), FireEffect, Start, (End - Start).Rotation());
}*/


/*void UDragonFireBreathComponent::EmitFire()
{
	UE_LOG(LogTemp, Warning, TEXT("🔥 FIRE EMITTED"));

	ACharacter* Dragon = Cast<ACharacter>(GetOwner());
	if (!Dragon || !CurrentTarget || !FireEffect) return;

	USkeletalMeshComponent* Mesh = Dragon->GetMesh();
	if (!Mesh) return;

	UNiagaraFunctionLibrary::SpawnSystemAttached(
		FireEffect,
		Mesh,
		MouthSocket,
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		EAttachLocation::SnapToTarget,
		true);
}*/

void UDragonFireBreathComponent::EmitFire()
{
	ACharacter* Dragon = Cast<ACharacter>(GetOwner());
	if (!Dragon || !CurrentTarget || !FireEffect) return;

	FVector Start = Dragon->GetMesh()->GetSocketLocation(MouthSocket);
	FVector Dir = (CurrentTarget->GetActorLocation() - Start).GetSafeNormal();

	ADragonFlameBeam* Beam = GetWorld()->SpawnActor<ADragonFlameBeam>();
	Beam->SetFireFX(FireEffect);
	Beam->Init(Start, Dir);
}

