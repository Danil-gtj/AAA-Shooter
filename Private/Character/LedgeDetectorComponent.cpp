// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/LedgeDetectorComponent.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "GameCode/GameCodeTypes.h"
#include "DrawDebugHelpers.h"

void ULedgeDetectorComponent::BeginPlay()
{
	Super::BeginPlay();
	checkf(GetOwner()->IsA<ACharacter>(), TEXT(" ULedgeDetectorComponent::BeginPlay() only a character can use UladgeDetectorComponent"));
	CachedCharacterOwner = StaticCast<ACharacter*>(GetOwner());
}

bool ULedgeDetectorComponent::DetectLedge(OUT FLedgeDescription& LedgeDescription)
{
	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = true;
	QueryParams.AddIgnoredActor(GetOwner());

	UCapsuleComponent* CapsuleComponent = CachedCharacterOwner->GetCapsuleComponent();

	float BottomZOffset = 2.0f;
	FVector ChracterBottom = CachedCharacterOwner->GetActorLocation() - (CapsuleComponent->GetUnscaledCapsuleHalfHeight() - BottomZOffset) * FVector::UpVector;

	float ForwardCheckCapsuleRaius = CapsuleComponent->GetUnscaledCapsuleRadius();
	float ForwardCheckCapsuleHalfHeight = (MaxLedgeHeight - MinLedgeHeight) * 0.5f;

	FHitResult ForwardCheckHitResult;
	FCollisionShape ForwardCapsuleShape = FCollisionShape::MakeCapsule(ForwardCheckCapsuleRaius, ForwardCheckCapsuleHalfHeight);
	FVector ForwardStartLocation = ChracterBottom + (MinLedgeHeight + ForwardCheckCapsuleHalfHeight) * FVector::UpVector;
	FVector ForwardEndLocation = ForwardStartLocation + CachedCharacterOwner->GetActorForwardVector() * ForwardCheckDistance;
	
	DrawDebugCapsule(GetWorld(), ForwardStartLocation, ForwardCheckCapsuleHalfHeight, ForwardCheckCapsuleRaius, FQuat::Identity, FColor::Black, false, 2.0f);
	DrawDebugCapsule(GetWorld(), ForwardEndLocation, ForwardCheckCapsuleHalfHeight, ForwardCheckCapsuleRaius, FQuat::Identity, FColor::Black, false, 2.0f);
	DrawDebugLine(GetWorld(), ForwardStartLocation, ForwardEndLocation, FColor::Black, false, 2.0f);
	if(!GetWorld()->SweepSingleByChannel(ForwardCheckHitResult, ForwardStartLocation, ForwardEndLocation, FQuat::Identity, ECC_Climbing, ForwardCapsuleShape, QueryParams)) 
	{
		return false;
	}
	DrawDebugCapsule(GetWorld(), ForwardCheckHitResult.Location, ForwardCheckCapsuleHalfHeight, ForwardCheckCapsuleRaius, FQuat::Identity, FColor::Red, false, 2.0f);
	DrawDebugPoint(GetWorld(), ForwardCheckHitResult.ImpactPoint, 10.0f, FColor::Red, false, 2.0f);

	FHitResult DownwardCheckHitResult;
	float DownwardSphereTraceRadius = CapsuleComponent->GetUnscaledCapsuleRadius();
	FCollisionShape DownwardSphereShape = FCollisionShape::MakeSphere(DownwardSphereTraceRadius);

	float DownwardCheckDepthOffset = 10.0f;
	FVector DownwardTraceStartLocation = ForwardCheckHitResult.ImpactPoint - ForwardCheckHitResult.ImpactNormal * DownwardCheckDepthOffset;
	DownwardTraceStartLocation.Z = ChracterBottom.Z + MaxLedgeHeight + DownwardSphereTraceRadius;
	FVector DownwardTraceEndLocation(DownwardTraceStartLocation.X, DownwardTraceStartLocation.Y, ChracterBottom.Z);

	FVector DebugDrawCapsuleLocation= (DownwardTraceStartLocation + DownwardTraceEndLocation) * 0.5f;
	float DebugDrawCapsuleHalfHeight = (DownwardTraceStartLocation - DownwardTraceEndLocation).Size() * 0.5f;

	DrawDebugCapsule(GetWorld(), DebugDrawCapsuleLocation, DebugDrawCapsuleHalfHeight, DownwardSphereTraceRadius, FQuat::Identity, FColor::Red, false, 2.0f);

	if (!GetWorld()->SweepSingleByChannel(DownwardCheckHitResult, DownwardTraceStartLocation, DownwardTraceEndLocation, FQuat::Identity, ECC_Climbing, DownwardSphereShape, QueryParams))
	{
		return false;
	}

	DrawDebugSphere(GetWorld(), DownwardCheckHitResult.Location, DownwardSphereTraceRadius, 32, FColor::Red, false, 2.0f);
	DrawDebugPoint(GetWorld(), DownwardCheckHitResult.ImpactPoint, 10.0f, FColor::Red, false, 2.0f);

	float OverlapCapsuleRadius = CapsuleComponent->GetUnscaledCapsuleRadius();
	float OverlapCapsuleHalfHeight = CapsuleComponent->GetUnscaledCapsuleHalfHeight();
	FCollisionShape OverlapCapsuleSphere = FCollisionShape::MakeCapsule(OverlapCapsuleRadius, OverlapCapsuleHalfHeight);
	float OverlapCapsuleFloorOffset = 2.0f;

	FVector OverlapLocation = DownwardCheckHitResult.ImpactPoint + (OverlapCapsuleHalfHeight + OverlapCapsuleFloorOffset) * FVector::UpVector;
	if (GetWorld()->OverlapBlockingTestByProfile(OverlapLocation, FQuat::Identity, FName("Pawn"), OverlapCapsuleSphere, QueryParams)) 
	{
		DrawDebugCapsule(GetWorld(), OverlapLocation, OverlapCapsuleHalfHeight, OverlapCapsuleRadius, FQuat::Identity, FColor::Red, false, 2.0f);
		return false;
	}

	LedgeDescription.Component = DownwardCheckHitResult.GetComponent();
	LedgeDescription.Location = OverlapLocation - LedgeDescription.Component->GetComponentLocation();
	LedgeDescription.Rotation = (ForwardCheckHitResult.ImpactNormal * FVector(-1.0f, -1.0f, 0.0f)).ToOrientationRotator();
	LedgeDescription.LedgeNoramal = ForwardCheckHitResult.ImpactNormal;

	return true;
}


