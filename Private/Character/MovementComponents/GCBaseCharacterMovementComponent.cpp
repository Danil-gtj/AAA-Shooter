// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/MovementComponents/GCBaseCharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "Character/LedgeDetectorComponent.h"
#include "Curves/CurveVector.h"
#include "Actors/Environment/Ladder.h"
#include "Zipline.h"

FNetworkPredictionData_Client* UGCBaseCharacterMovementComponent::GetPredictionData_Client() const
{
	if (ClientPredictionData == nullptr) 
	{
		UGCBaseCharacterMovementComponent* MutableThis = const_cast<UGCBaseCharacterMovementComponent*>(this);
		MutableThis->ClientPredictionData = new FNetworkPredictionData_Client_Character_GC(*this);
	}
	return ClientPredictionData;
}

void UGCBaseCharacterMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);

	bool bWasMantling = GetBaseCharacterOwner()->bIsMantling;
	bIsSprinting = (Flags & FSavedMove_Character::FLAG_Custom_0) != 0;
	bool bIsMantling = (Flags & FSavedMove_Character::FLAG_Custom_1) != 0;
	
	if (GetBaseCharacterOwner()->GetLocalRole() == ROLE_Authority) 
	{
		if (!bWasMantling && bIsMantling) 
		{
			GetBaseCharacterOwner()->Mantle(true);
		} 
	}

}

void UGCBaseCharacterMovementComponent::PhysicsRotation(float DeltaTime)
{
	if (bForceRotation) 
	{
		FRotator CurrentRotation = UpdatedComponent->GetComponentRotation();

		FRotator DeltaRot = GetDeltaRotation(DeltaTime);

		const float AngleTolerance = 1e-3f;

		if (!CurrentRotation.Equals(ForcedTargetRotation, AngleTolerance)) 
		{
			FRotator DesiredRotaion = ForcedTargetRotation;

			if (!FMath::IsNearlyEqual(CurrentRotation.Pitch, DesiredRotaion.Pitch, AngleTolerance))
			{
				DesiredRotaion.Pitch = FMath::FixedTurn(CurrentRotation.Pitch, DesiredRotaion.Pitch, DeltaRot.Pitch);
			}

			if (!FMath::IsNearlyEqual(CurrentRotation.Yaw, DesiredRotaion.Yaw, AngleTolerance)) 
			{
				DesiredRotaion.Yaw = FMath::FixedTurn(CurrentRotation.Yaw, DesiredRotaion.Yaw, DeltaRot.Yaw);
			}

			if (!FMath::IsNearlyEqual(CurrentRotation.Roll, DesiredRotaion.Roll, AngleTolerance))
			{
				DesiredRotaion.Roll = FMath::FixedTurn(CurrentRotation.Roll, DesiredRotaion.Roll, DeltaRot.Roll);
			}

			MoveUpdatedComponent(FVector::ZeroVector, DesiredRotaion, false);
		}
		else
		{
			ForcedTargetRotation = FRotator::ZeroRotator;
			bForceRotation = false;
		}
		
		return;
	}

	if (IsOnLadder()) 
	{
		return;
	}
	Super::PhysicsRotation(DeltaTime);
}

float UGCBaseCharacterMovementComponent::GetMaxSpeed() const
{
	float Result = Super::GetMaxSpeed();
	if (bIsSprinting)
	{
		Result = SprintSpeed;
	}
	else if (IsOnLadder()) 
	{
		Result = ClimbingOnLadderSpeed;
	}
	else if (GetBaseCharacterOwner()->IsAiming()) 
	{
		Result = GetBaseCharacterOwner()->GetAimingMovementSpeed();
	}
	return Result;
}

void UGCBaseCharacterMovementComponent::StartSprint()
{
	bIsSprinting = true;
	bForceMaxAccel = 1;
}

void UGCBaseCharacterMovementComponent::StopSprint()
{
	bIsSprinting = false;
	bForceMaxAccel = 0;
}

void UGCBaseCharacterMovementComponent::AttachToZipline(const class AZipline* Ladder)
{
	if (IsOnZipline())
	{
		return;
	}

	const AZipline* AvailableZipline = GetBaseCharacterOwner()->GetAvaliableZipline();

	CurrentZipline = AvailableZipline;

	FRotator TargetOrientationRotator = CurrentZipline->GetActorForwardVector().ToOrientationRotator();

	FVector NewCharacterLocation = CurrentZipline->StartZiplineVector;
	GetOwner()->SetActorLocation(NewCharacterLocation);
	GetOwner()->SetActorRotation(TargetOrientationRotator);
	
	SetMovementMode(EMovementMode::MOVE_Custom, (uint8)ECustomMovementMode::CMOVE_Zipline);
}

void UGCBaseCharacterMovementComponent::StartMantle(const FMantlingMovementParameters & MantlingParameters)
{
	if (IsMantling())
	{
		return;
	}
	CurrentMantlingParameters = MantlingParameters;
	SetMovementMode(EMovementMode::MOVE_Custom, (uint8)ECustomMovementMode::CMOVE_Mantling);
}

void UGCBaseCharacterMovementComponent::EndMantle()
{
	GetBaseCharacterOwner()->bIsMantling = false;
	SetMovementMode(MOVE_Walking);
}

bool UGCBaseCharacterMovementComponent::IsMantling() const
{
	return UpdatedComponent && MovementMode == MOVE_Custom && CustomMovementMode == (uint8)ECustomMovementMode::CMOVE_Mantling;
}

bool UGCBaseCharacterMovementComponent::IsOnZipline()
{
	return UpdatedComponent && MovementMode == MOVE_Custom && CustomMovementMode == (uint8)ECustomMovementMode::CMOVE_Zipline;
}

void UGCBaseCharacterMovementComponent::DetachFromZipline()
{
	SetMovementMode(MOVE_Falling);
}

void UGCBaseCharacterMovementComponent::AttachToLadder(const ALadder* Ladder)
{
	const ALadder* AvailableLadder = GetBaseCharacterOwner()->GetAvailableLadder();
	if (!IsValid(AvailableLadder))
	{
		return;
	}
	CurrentLadder = Ladder;
	
	FRotator TargetOrientationRotator = CurrentLadder->GetActorForwardVector().ToOrientationRotator();
	TargetOrientationRotator.Yaw += 180.0f;

	FVector LadderUpVector = CurrentLadder->GetActorUpVector();
	FVector LadderForwardVector = CurrentLadder->GetActorForwardVector();
	float Projection = GetCharacterToCurrentLadderProjection(GetActorLocation());

	FVector NewCharacterLocation = CurrentLadder->GetActorLocation() + Projection * LadderUpVector + LadderToCharacterOffset * LadderForwardVector;

	if (CurrentLadder->GetIsOnTop()) 
	{
		NewCharacterLocation = CurrentLadder->GetAttachFromTopAnimMontageStartingLocation();
	}

	GetOwner()->SetActorLocation(NewCharacterLocation);
	GetOwner()->SetActorRotation(TargetOrientationRotator);

	SetMovementMode(MOVE_Custom, (uint8)ECustomMovementMode::CMOVE_OnLadder);
}

void UGCBaseCharacterMovementComponent::DetachFromladder(EDetachFromLadderMethod EDetachFromLadderMethod)
{
	switch (EDetachFromLadderMethod)
	{
	case EDetachFromLadderMethod::Fall:
	default:
	{
		SetMovementMode(MOVE_Falling);
		break;
	}
	case EDetachFromLadderMethod::ReachingTheBottom:
	{
		SetMovementMode(MOVE_Walking);
		break;
	}
	case EDetachFromLadderMethod::ReachingTheTop:
	{
		GetBaseCharacterOwner()->Mantle(true);
		break;
	}
	case EDetachFromLadderMethod::JumpOff:
	{
		FVector JumpDerection = CurrentLadder->GetActorForwardVector();
		SetMovementMode(MOVE_Falling);

		FVector JumpVelocity = JumpDerection * JumpOffFromLadderSpeed;

		ForcedTargetRotation = JumpDerection.ToOrientationRotator();
		bForceRotation = true;

		Launch(JumpVelocity);
		break;
	}
	}
}

bool UGCBaseCharacterMovementComponent::IsOnLadder() const
{
	return UpdatedComponent && MovementMode == MOVE_Custom && CustomMovementMode == (uint8)ECustomMovementMode::CMOVE_OnLadder;
}

const ALadder* UGCBaseCharacterMovementComponent::GetCurrenLadder()
{
	return CurrentLadder;
}

float UGCBaseCharacterMovementComponent::GetLadderSpeedRatio() const	
{
	checkf(IsValid(CurrentLadder), TEXT("float UGCBaseCharacterMovementComponent::GetCharacterToCurrentLadderProjection()"));
	
	FVector LadderUpVector = CurrentLadder->GetActorUpVector();
	return FVector::DotProduct(LadderUpVector, Velocity) / ClimbingOnLadderSpeed;
}

void UGCBaseCharacterMovementComponent::SetIsOutOfStamina(bool bIsOutOfStamina_In)
{
	bIsOutOfStamina = bIsOutOfStamina_In;
	if (bIsOutOfStamina) 
	{
		bIsSprinting = false;
		bForceMaxAccel = 0;
	}
}

void UGCBaseCharacterMovementComponent::PhysCustom(float DeltaTime, int32 Iterations)
{
	if (GetBaseCharacterOwner()->GetLocalRole() == ROLE_SimulatedProxy)
	{
		return;
	}

	switch (CustomMovementMode)
	{
	case (uint8)ECustomMovementMode::CMOVE_Mantling:
	{
		PhysMantling(DeltaTime, Iterations);
		break;
	}
	case (uint8)ECustomMovementMode::CMOVE_OnLadder:
	{
		PhysLadder(DeltaTime, Iterations);
		break;
	}
	case (uint8)ECustomMovementMode::CMOVE_Zipline:
	{
		PhysZipline(DeltaTime, Iterations);
		break;
	}
	default:
		break;
	}

	Super::PhysCustom(DeltaTime, Iterations);
}

void UGCBaseCharacterMovementComponent::PhysMantling(float DeltaTime, int32 Iterations)
{
	float ElapsedTime = GetWorld()->GetTimerManager().GetTimerElapsed(MantlingTimer) + CurrentMantlingParameters.StartTime;

	FVector MantlingCurveValue = CurrentMantlingParameters.MantlingCurve->GetVectorValue(ElapsedTime);

	float PositionAlpha = MantlingCurveValue.X;
	float XYCorrectionAlpha = MantlingCurveValue.Y;
	float ZCorrectionAlpha = MantlingCurveValue.Z;

	FVector CorrectedInitialLocation = FMath::Lerp(CurrentMantlingParameters.InitialLocation, CurrentMantlingParameters.InitialAnimationLocation, XYCorrectionAlpha);
	CorrectedInitialLocation.Z = FMath::Lerp(CurrentMantlingParameters.InitialLocation.Z, CurrentMantlingParameters.InitialAnimationLocation.Z, ZCorrectionAlpha);

	FVector NewLocation = FMath::Lerp(CorrectedInitialLocation, CurrentMantlingParameters.TargetLocation + CurrentMantlingParameters.Component->GetComponentLocation(), PositionAlpha);
	FRotator NewRotation = FMath::Lerp(CurrentMantlingParameters.InitialRotation, CurrentMantlingParameters.TargetRotation, PositionAlpha);

	FVector Delta = NewLocation - GetActorLocation();
	Velocity = Delta / DeltaTime;

	FHitResult Hit;
	SafeMoveUpdatedComponent(Delta, NewRotation, false, Hit);
}

void UGCBaseCharacterMovementComponent::PhysLadder(float DeltaTime, int32 Iterations)
{
	CalcVelocity(DeltaTime, 1.0f, false, ClimbingOnLadderBrealingDeceleration);
	FVector Delta = Velocity * DeltaTime;

	if (HasAnimRootMotion()) 
	{
		FHitResult Hit;
		SafeMoveUpdatedComponent(Delta, GetOwner()->GetActorRotation(), true, Hit);
		return;
	}

	FVector NewPos = GetActorLocation() + Delta;
	float NewPosProjection = GetCharacterToCurrentLadderProjection(NewPos);

	if (NewPosProjection < MinLadderBottomOffset)
	{
		DetachFromladder(EDetachFromLadderMethod::ReachingTheBottom);
		return;
	}
	else if (NewPosProjection > (CurrentLadder->GetLadderHeight() - MaxLadderTopOffset))
	{
		DetachFromladder(EDetachFromLadderMethod::ReachingTheTop);
		return;
	}

	FHitResult Hit;
	SafeMoveUpdatedComponent(Delta, GetOwner()->GetActorRotation(), true, Hit);
}
 
void UGCBaseCharacterMovementComponent::PhysZipline(float DeltaTime, int32 Iteraions)
{
	CharacterOwner->SetActorLocation(FMath::Lerp(CurrentZipline->StartZiplineVector, CurrentZipline->EndZiplineVector,  SpeedOnZipline * 0.01f));
}

bool UGCBaseCharacterMovementComponent::IsProning() const
{
	return  GetBaseCharacterOwner()->bIsProned;
}

void UGCBaseCharacterMovementComponent::Prone(bool bClientSimulation)
{
	if (!HasValidData())
	{
		return;
	}

	if (!CanProneInCurrentState()) 
	{
		return;
	}

	if (GetBaseCharacterOwner()->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() == ProneCapsuleHalfHeight) 
	{
		if (!bClientSimulation) 
		{
			GetBaseCharacterOwner()->bIsProned = false;
			bWantsToProne = false;
			GetBaseCharacterOwner()->OnStartProne(0.0f, 0.0f);
			return;
		}
	}

	const float ComponentScale = CharacterOwner->GetCapsuleComponent()->GetShapeScale();
	const float OldUnscaleHalfHeight = CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	const float OldUnscaleRadius = CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleRadius();

	const float ClampedProneHalfHeight = FMath::Max3(0.0f, OldUnscaleRadius, PronedHalfHeight);
	CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(OldUnscaleRadius, OldUnscaleHalfHeight);
	float HalfHeightAdjust = (OldUnscaleHalfHeight - ClampedProneHalfHeight);
	float ScaledHalfHeightAdjust = HalfHeightAdjust * ComponentScale;

	if (!bClientSimulation)
	{
		if (ClampedProneHalfHeight > OldUnscaleHalfHeight) 
		{
			FCollisionQueryParams CapsuleParams(SCENE_QUERY_STAT(ProneTrace), false, CharacterOwner);
			FCollisionResponseParams ResponceParam;
			InitCollisionParams(CapsuleParams, ResponceParam);
			const bool bEncrouched = GetWorld()->OverlapBlockingTestByChannel(UpdatedComponent->GetComponentLocation() - FVector(0.0f, 0.0f, ScaledHalfHeightAdjust), FQuat::Identity, UpdatedComponent->GetCollisionObjectType(), GetPawnCapsuleCollisionShape(SHRINK_None), CapsuleParams, ResponceParam);

			if (bEncrouched)
			{
				CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(OldUnscaleRadius, OldUnscaleHalfHeight);
				return;
			}
		}

		if (bProneMaintainsBaseLocation) 
		{
			UpdatedComponent->MoveComponent(FVector(0.0f, 0.0f, -ScaledHalfHeightAdjust), UpdatedComponent->GetComponentQuat(), true, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags);
		}

		GetBaseCharacterOwner()->bIsProned = true;
		bWantsToCrouch = false;
		GetBaseCharacterOwner()->bIsCrouched = false;
	}

	bForceNextFloorCheck = true;

	ACharacter* DefaultCharacter = GetBaseCharacterOwner()->GetClass()->GetDefaultObject<ACharacter>();
	HalfHeightAdjust = (DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() - CrouchedHalfHeight - ClampedProneHalfHeight);
	ScaledHalfHeightAdjust = HalfHeightAdjust * ComponentScale;

	AdjustProxyCapsuleSize();
	GetBaseCharacterOwner()->OnStartProne(HalfHeightAdjust, ScaledHalfHeightAdjust);
}

void UGCBaseCharacterMovementComponent::UnProne(bool bClientSimulation)
{
	if (!HasValidData())
	{
		return;
	}

	ACharacter* DefaultCharacter = GetBaseCharacterOwner()->GetClass()->GetDefaultObject<ACharacter>();
	float DesiredHeight = bIsFullHeight ? DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() : CrouchedHalfHeight;

	if (GetBaseCharacterOwner()->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() == DesiredHeight) 
	{
		if (!bClientSimulation) 
		{
			GetBaseCharacterOwner()->bIsProned = false;
			bWantsToCrouch = !bIsFullHeight;
			GetBaseCharacterOwner()->bIsCrouched = !bIsFullHeight;
		}
		GetBaseCharacterOwner()->OnEndProne(0.0f, 0.0f);
		return;
	}

	const float CurrentPronedHalfHeight = GetBaseCharacterOwner()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	const float ComponentScale = CharacterOwner->GetCapsuleComponent()->GetShapeScale();
	const float OldUnscaleHalfHeight = CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	float HalfHeightAdjust = DesiredHeight - OldUnscaleHalfHeight;
	float ScaledHalfHeightAdjust = HalfHeightAdjust * ComponentScale;
	const FVector PawnLocation = UpdatedComponent->GetComponentLocation();

	check(GetBaseCharacterOwner()->GetCapsuleComponent());

	if (!bClientSimulation) 
	{
		const UWorld* MyWorld = GetWorld();
		const float SweepInflation = KINDA_SMALL_NUMBER * 10.0f;
		FCollisionQueryParams CapsuleParams(SCENE_QUERY_STAT(ProneTrace), false, GetBaseCharacterOwner());
		FCollisionResponseParams ResponseParam;
		InitCollisionParams(CapsuleParams, ResponseParam);

		const FCollisionShape StandingCapsuleShape = GetPawnCapsuleCollisionShape(SHRINK_HeightCustom, -SweepInflation - ScaledHalfHeightAdjust);
		const ECollisionChannel CollisionChannel = UpdatedComponent->GetCollisionObjectType();
		bool bEncrouched = true;

		if (!bProneMaintainsBaseLocation) 
		{
			bEncrouched = MyWorld->OverlapBlockingTestByChannel(PawnLocation, FQuat::Identity, CollisionChannel, StandingCapsuleShape, CapsuleParams, ResponseParam);

			if (bEncrouched) 
			{
				if (ScaledHalfHeightAdjust > 0.0f) 
				{
					float PawnRadius, PawnHalfHeight;
					GetBaseCharacterOwner()->GetCapsuleComponent()->GetScaledCapsuleSize(PawnRadius, PawnHalfHeight);
					const float ShrinkHalfHeight = PawnHalfHeight - PawnRadius;
					const float TraceDist = PawnHalfHeight - ShrinkHalfHeight;
					const FVector Down = FVector(0.0f, 0.0f, -TraceDist);

					FHitResult Hit(1.0f);
					const FCollisionShape ShortCapsuleShape = GetPawnCapsuleCollisionShape(SHRINK_HeightCustom, ShrinkHalfHeight);
					const bool bBlockingHit = MyWorld->SweepSingleByChannel(Hit, PawnLocation, PawnLocation + Down, FQuat::Identity, CollisionChannel, StandingCapsuleShape, CapsuleParams);
					if (Hit.bStartPenetrating) 
					{
						bEncrouched = true;
					}
					else
					{
						const float DistanceToBase = (Hit.Time * TraceDist) + ShortCapsuleShape.Capsule.HalfHeight;
						const FVector NewLoc = FVector(PawnLocation.X, PawnLocation.Y, PawnLocation.Z - DistanceToBase + StandingCapsuleShape.Capsule.HalfHeight);
						bEncrouched = MyWorld->OverlapBlockingTestByChannel(NewLoc, FQuat::Identity, CollisionChannel, StandingCapsuleShape, CapsuleParams, ResponseParam);
						if (!bEncrouched) 
						{
							UpdatedComponent->MoveComponent(NewLoc - PawnLocation, UpdatedComponent->GetComponentQuat(), false, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
						}
					}
				}
			}
		}
		else 
		{
			FVector StandingLocation = PawnLocation + FVector(0.0f, 0.0f, StandingCapsuleShape.GetCapsuleHalfHeight() - CurrentPronedHalfHeight);
			bEncrouched = MyWorld->OverlapBlockingTestByChannel(StandingLocation, FQuat::Identity, CollisionChannel, StandingCapsuleShape, CapsuleParams, ResponseParam);

			if (bEncrouched) 
			{
				if (IsMovingOnGround()) 
				{
					const float MinFlootDist = KINDA_SMALL_NUMBER * 10.0f;
					if (CurrentFloor.bBlockingHit && CurrentFloor.FloorDist > MinFlootDist) 
					{
						StandingLocation.Z -= CurrentFloor.FloorDist - MinFlootDist;
						bEncrouched = MyWorld->OverlapBlockingTestByChannel(StandingLocation, FQuat::Identity, CollisionChannel, StandingCapsuleShape, CapsuleParams, ResponseParam);
					}
				}
			}

			if (!bEncrouched) 
			{
				UpdatedComponent->MoveComponent(StandingLocation - PawnLocation, UpdatedComponent->GetComponentQuat(), false, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
				bForceNextFloorCheck = true;
			}
		}

		if (bEncrouched) 
		{
			return;
		}

		GetBaseCharacterOwner()->bIsProned = false;
		bWantsToCrouch = !bIsFullHeight;
		GetBaseCharacterOwner()->bIsCrouched = !bIsFullHeight;
	}
	else 
	{
		bShrinkProxyCapsule = true;
	}

	GetBaseCharacterOwner()->GetCapsuleComponent()->SetCapsuleSize(DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleRadius(), DesiredHeight, true);

	HalfHeightAdjust = bIsFullHeight ? CrouchedHalfHeight - PronedHalfHeight : DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() - DesiredHeight - PronedHalfHeight;

	AdjustProxyCapsuleSize();
	GetBaseCharacterOwner()->OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
}

void UGCBaseCharacterMovementComponent::UpdateCharacterStateBeforeMovement(float DeltaSeconds)
{
	Super::UpdateCharacterStateBeforeMovement(DeltaSeconds);

	if (GetCharacterOwner()->GetLocalRole() != ROLE_SimulatedProxy) 
	{
		const bool bIsProning = IsProning();
		if (bIsProning && (!bWantsToProne || !CanProneInCurrentState()))
		{
			UnProne(false);
		}
		else if (!bIsProning && bWantsToProne && CanProneInCurrentState())
		{
			Prone(false);
		}
	}
}

bool UGCBaseCharacterMovementComponent::CanProneInCurrentState() const
{
	return IsMovingOnGround() && UpdatedComponent && !UpdatedComponent;
}

void UGCBaseCharacterMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);
	if (MovementMode == MOVE_Swimming)
	{
		CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(SwimmingCapsuleRadius, SwimmingCapsuleHalfHeight);
	}
	else if(PreviousMovementMode == MOVE_Swimming)
	{
		ACharacter* DefaultCharacter = CharacterOwner->GetClass()->GetDefaultObject<ACharacter>();
		CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleRadius(), DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight(), true);
	}

	if (PreviousMovementMode == MOVE_Custom && PreviousCustomMode == (uint8)ECustomMovementMode::CMOVE_OnLadder)
	{
		CurrentLadder = nullptr;
	}

	if (MovementMode == MOVE_Custom) 
	{
		switch(CustomMovementMode)
		{
		case (uint8)ECustomMovementMode::CMOVE_Mantling:
		{	
			GetWorld()->GetTimerManager().SetTimer(MantlingTimer, this, &UGCBaseCharacterMovementComponent::EndMantle, CurrentMantlingParameters.Duration, false);
			break;
		}
		default:
			break;
		}
	}
}

AGCBaseCharacter* UGCBaseCharacterMovementComponent::GetBaseCharacterOwner() const
{
	return Cast<AGCBaseCharacter>(CharacterOwner);
}

float UGCBaseCharacterMovementComponent::GetCharacterToCurrentLadderProjection(const FVector& Location)
{
	checkf(IsValid(CurrentLadder), TEXT("float UGCBaseCharacterMovementComponent::GetCharacterToCurrentLadderProjection()"));

	FVector LadderUpVector = CurrentLadder->GetActorUpVector();
	FVector LadderToCharacterDistance = Location - CurrentLadder->GetActorLocation();
	return FVector::DotProduct(LadderUpVector, LadderToCharacterDistance);
}

void FSavedMove_GC::Clear()
{
	Super::Clear();
	bSavedIsSprinting = 0;
	bSavedIsMantling = 0;
}

uint8 FSavedMove_GC::GetCompressedFlags() const
{
	uint8 Result = Super::GetCompressedFlags();
	
	if (bSavedIsSprinting) 
	{  
		Result |= FLAG_Custom_0;
	}
	if (bSavedIsMantling)
	{
		Result &= ~FLAG_JumpPressed;
		Result |= FLAG_Custom_1;
	}
	return Result;
}

bool FSavedMove_GC::CanCombineWith(const FSavedMovePtr& NewMovePtr, ACharacter* InCharacter, float MaxDelta) const
{
	const FSavedMove_GC* NewMove = StaticCast<const FSavedMove_GC*>(NewMovePtr.Get());

	if (bSavedIsSprinting != NewMove->bSavedIsSprinting || bSavedIsMantling != NewMove->bSavedIsMantling) 
	{
		return false;
	}

	return Super::CanCombineWith(NewMovePtr, InCharacter, MaxDelta);
}

void FSavedMove_GC::SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData)
{
	Super::SetMoveFor(Character, InDeltaTime, NewAccel, ClientData);

	check(Character->IsA<AGCBaseCharacter>());
	AGCBaseCharacter* InBaseChracter = StaticCast<AGCBaseCharacter*>(Character);
	UGCBaseCharacterMovementComponent* MovementComponent = InBaseChracter->GetBaseCharacterMovementComponent();

	bSavedIsSprinting = MovementComponent->bIsSprinting;
	bSavedIsMantling = InBaseChracter->bIsMantling;

}

void FSavedMove_GC::PrepMoveFor(ACharacter* Character)
{
	Super::PrepMoveFor(Character);

	UGCBaseCharacterMovementComponent* MovementComponent = StaticCast<UGCBaseCharacterMovementComponent*>(Character->GetMovementComponent());

	MovementComponent->bIsSprinting = bSavedIsSprinting; 
}

FNetworkPredictionData_Client_Character_GC::FNetworkPredictionData_Client_Character_GC(const UCharacterMovementComponent& ClientMovement)
	: Super(ClientMovement)
{

}

FSavedMovePtr FNetworkPredictionData_Client_Character_GC::AllocateNewMove()
{
	return FSavedMovePtr(new FSavedMove_GC());
}