// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/FPPlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameCode/GameCodeTypes.h"
#include "Character/GCPlayerController.h"
#include "Character/MovementComponents/GCBaseCharacterMovementComponent.h"

AFPPlayerCharacter::AFPPlayerCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	FirstPersonMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FPS Arm"));
	FirstPersonMeshComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonMeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, -86.0f));
	FirstPersonMeshComponent->CastShadow = false;
	FirstPersonMeshComponent->bCastDynamicShadow = false;
	FirstPersonMeshComponent->SetOnlyOwnerSee(true);

	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FPS Camera"));
	FirstPersonCameraComponent->SetupAttachment(FirstPersonMeshComponent, SocketFPCamera);
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	GetMesh()->SetOwnerNoSee(true);
	GetMesh()->bCastHiddenShadow = true;

	CameraComponent->bAutoActivate = false;

	SpringArmComponent->bAutoActivate = false;
	SpringArmComponent->bUsePawnControlRotation = false;

	GetCharacterMovement()->bOrientRotationToMovement = false;

	bUseControllerRotationYaw = true;
}

void AFPPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	GCPlayerController = Cast<AGCPlayerController>(NewController);
}

void AFPPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (IsFPMontagePlaying() && GCPlayerController.IsValid())
	{
		FRotator TargetControllRotation = GCPlayerController->GetControlRotation();
		TargetControllRotation.Pitch = 0.0f;
		float BlendSpeed = 300.0f;
		TargetControllRotation = FMath::RInterpTo(GCPlayerController->GetControlRotation(), TargetControllRotation, DeltaTime, BlendSpeed);
		GCPlayerController->SetControlRotation(TargetControllRotation);
	}
}

void AFPPlayerCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	const AFPPlayerCharacter* DefaultCharacter = GetDefault<AFPPlayerCharacter>(GetClass());
	FVector& FirstPersonMeshRelativeLocation = FirstPersonMeshComponent->GetRelativeLocation_DirectMutable();
	FirstPersonMeshRelativeLocation.Z = DefaultCharacter->FirstPersonMeshComponent->GetRelativeLocation().Z + HalfHeightAdjust;
}

void AFPPlayerCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	const AFPPlayerCharacter* DefaultCharacter = GetDefault<AFPPlayerCharacter>(GetClass());
	FVector& FirstPersonMeshRelativeLocation = FirstPersonMeshComponent->GetRelativeLocation_DirectMutable();
	FirstPersonMeshRelativeLocation.Z = DefaultCharacter->FirstPersonMeshComponent->GetRelativeLocation().Z;
}

FRotator AFPPlayerCharacter::GetViewRotation() const
{
	FRotator Result = Super::GetViewRotation();

	if(IsFPMontagePlaying())
	{
		FRotator SocketRotation = FirstPersonMeshComponent->GetSocketRotation(SocketFPCamera);
		Result.Yaw = SocketRotation.Yaw;
		Result.Roll = SocketRotation.Roll;
		Result.Pitch += SocketRotation.Pitch;
	}

	return Result;
}

void AFPPlayerCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);
	if (GetBaseCharacterMovementComponent()->IsOnLadder()) 
	{
		if (GCPlayerController.IsValid()) 
		{
			GCPlayerController->SetIgnoreCameraPitch(true);
			bUseControllerRotationYaw = false;
			APlayerCameraManager* CameraManager = GCPlayerController->PlayerCameraManager;
			CameraManager->ViewPitchMin = LadderCameraMinPitch;
			CameraManager->ViewPitchMax = LadderCameraMaxPitch;
			CameraManager->ViewYawMin = LadderCameraMinYaw;
			CameraManager->ViewYawMax = LadderCameraMaxYaw;
		}
		else if(PreviousCustomMode == (uint8)ECustomMovementMode::CMOVE_OnLadder)
		{
			if(GCPlayerController.IsValid())
			{
				GCPlayerController->SetIgnoreCameraPitch(false);
				bUseControllerRotationYaw = true;
				APlayerCameraManager* CameraManager = GCPlayerController->PlayerCameraManager;
				APlayerCameraManager* DefaultCameraManager = CameraManager->GetClass()->GetDefaultObject<APlayerCameraManager>();

				CameraManager->ViewPitchMin = DefaultCameraManager->ViewPitchMin;
				CameraManager->ViewPitchMax = DefaultCameraManager->ViewPitchMax;
				CameraManager->ViewYawMin = DefaultCameraManager->ViewYawMin;
				CameraManager->ViewYawMax = DefaultCameraManager->ViewYawMax;
			}
		}
	}
	
	if (GetBaseCharacterMovementComponent()->IsOnZipline())
	{
		if (GCPlayerController.IsValid())
		{
			GCPlayerController->SetIgnoreCameraPitch(true);
			bUseControllerRotationYaw = false;
			APlayerCameraManager* CameraManager = GCPlayerController->PlayerCameraManager;
			CameraManager->ViewPitchMin = ZiplineCameraMinPitch;
			CameraManager->ViewPitchMax = ZiplineCameraMaxPitch;
			CameraManager->ViewYawMin = ZiplineCameraMinYaw;
			CameraManager->ViewYawMax = ZiplineCameraMaxYaw;
		}
		else if (PreviousCustomMode == (uint8)ECustomMovementMode::CMOVE_Zipline)
		{
			if (GCPlayerController.IsValid())
			{
				GCPlayerController->SetIgnoreCameraPitch(false);
				bUseControllerRotationYaw = true;
				APlayerCameraManager* CameraManager = GCPlayerController->PlayerCameraManager;
				APlayerCameraManager* DefaultCameraManager = CameraManager->GetClass()->GetDefaultObject<APlayerCameraManager>();

				CameraManager->ViewPitchMin = DefaultCameraManager->ViewPitchMin;
				CameraManager->ViewPitchMax = DefaultCameraManager->ViewPitchMax;
				CameraManager->ViewYawMin = DefaultCameraManager->ViewYawMin;
				CameraManager->ViewYawMax = DefaultCameraManager->ViewYawMax;
			}
		}
	}
}

bool AFPPlayerCharacter::IsFPMontagePlaying() const
{
	UAnimInstance* FPAnimInstance = FirstPersonMeshComponent->GetAnimInstance();
	return IsValid(FPAnimInstance) && FPAnimInstance->IsAnyMontagePlaying();
}

void AFPPlayerCharacter::OnMantle(const FMantlingSettings& MantlingSettings, float MantlingAnimationStartTime)
{
	Super::OnMantle(MantlingSettings, MantlingAnimationStartTime);
	UAnimInstance* FPAnimInstance = FirstPersonMeshComponent->GetAnimInstance();
	if (IsValid(FPAnimInstance) && MantlingSettings.FPSMantlingMontage)
	{
		if (GCPlayerController.IsValid())
		{
			GCPlayerController->SetIgnoreLookInput(true);
			GCPlayerController->SetIgnoreMoveInput(true);
		}
		float MontageDuration = FPAnimInstance->Montage_Play(MantlingSettings.FPSMantlingMontage, 1.0f, EMontagePlayReturnType::Duration, MantlingAnimationStartTime);
		GetWorld()->GetTimerManager().SetTimer(MontageFPTimer, this, &AFPPlayerCharacter::OnFPMontageTimerElapsed, MontageDuration, false);
	}
}

void AFPPlayerCharacter::OnFPMontageTimerElapsed()
{
	if (GCPlayerController.IsValid())
	{
		GCPlayerController->SetIgnoreLookInput(false);
		GCPlayerController->SetIgnoreMoveInput(false);
	}
}
