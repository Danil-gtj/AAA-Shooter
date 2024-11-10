// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Weapon/MeleeHitRegistrator.h"
#include "GameCode/GameCodeTypes.h"

UMeleeHitRegistrator::UMeleeHitRegistrator()
{
	PrimaryComponentTick.bCanEverTick = true;
	SphereRadius = 5.0f;
	SetCollisionProfileName(CollisionProfileNoCollision);
}

void UMeleeHitRegistrator::ProcessHitRegistration()
{
	FVector CurrentLocation = GetComponentLocation();

	FHitResult HitResult;

	FCollisionShape ColShape = FCollisionShape::MakeSphere(SphereRadius);
	bool bHasHit = GetWorld()->SweepSingleByChannel(HitResult, PreviousComponentLocation, CurrentLocation, FQuat::Identity, ECC_Melee, ColShape);

	if (bHasHit) 
	{
		FVector Direction = (CurrentLocation - PreviousComponentLocation).GetSafeNormal();
		if (OnMeleeHitRegestration.IsBound())
		{
			OnMeleeHitRegestration.Broadcast(HitResult, Direction);
		}
	}
}

void UMeleeHitRegistrator::SetIsHitRegestrationEnabled(bool bIsEnabled_In)
{
	bIsHitRegistrationEnabled = bIsEnabled_In;
}

void UMeleeHitRegistrator::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (bIsHitRegistrationEnabled) 
	{
		ProcessHitRegistration();
	}
	PreviousComponentLocation = GetComponentLocation();
}
