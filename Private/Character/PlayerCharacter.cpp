// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Actors/Equipment/Weapons/RangeWeaponItem.h"
#include "Character/CharacterComponents/CharacterEquipmentComponent.h"

APlayerCharacter::APlayerCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring arm"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->bUsePawnControlRotation = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(SpringArmComponent);
	CameraComponent->bUsePawnControlRotation = false;

	GetCharacterMovement()->bOrientRotationToMovement = 1;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (IsValid(AimingCurve))
	{
		FOnTimelineFloatStatic AimingTimelineFunction;
		AimingTimelineFunction.BindUObject(this, &APlayerCharacter::AimingTimelineUpdate);
		AimingTimeline.AddInterpFloat(AimingCurve, AimingTimelineFunction);
	}
}

void APlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	AimingTimeline.TickTimeline(DeltaSeconds);
}

void APlayerCharacter::MoveForward(float Value)
{
	if ((GetCharacterMovement()->IsMovingOnGround() || GetCharacterMovement()->IsFalling()) && !FMath::IsNearlyZero(Value, 1e-6f))
	{
		FRotator YawRotator(0.0f, GetControlRotation().Yaw, 0.0f);
		FVector ForwardVector = YawRotator.RotateVector(FVector::ForwardVector);
		AddMovementInput(ForwardVector, Value);
	}
}

void APlayerCharacter::MoveRight(float Value)
{
	if ((GetCharacterMovement()->IsMovingOnGround() || GetCharacterMovement()->IsFalling()) && !FMath::IsNearlyZero(Value, 1e-6f))
	{
		FRotator YawRotator(0.0f, GetControlRotation().Yaw, 0.0f);
		FVector	RightVector = YawRotator.RotateVector(FVector::RightVector);
		AddMovementInput(RightVector, Value);
	}
}

void APlayerCharacter::Turn(float Value)
{
	ARangeWeaponItem* CurrentRangeWeapon = GetCharacterEquipmentComponent()->GetCurrentRrangeWeapon();
	float Modifier;
	if (IsValid(CurrentRangeWeapon)) 
	{
		Modifier = CurrentRangeWeapon->GetAimingTurnModifier() * BaseTurnRate * GetWorld()->GetDeltaSeconds();
	}
	else
	{
		Modifier = 1.0f;
	}
	AddControllerYawInput(Value * Modifier);
}

void APlayerCharacter::LookUp(float Value)
{
	ARangeWeaponItem* CurrentRangeWeapon = GetCharacterEquipmentComponent()->GetCurrentRrangeWeapon();
	float Modifier;
	if (IsValid(CurrentRangeWeapon))
	{
		Modifier = CurrentRangeWeapon->GetAimingLookUpModifier();
	}
	else
	{
		Modifier = 1.0f;
	}
	AddControllerPitchInput(Value * Modifier);
}

void APlayerCharacter::TurnAtRate(float Value)
{
	AddControllerYawInput(Value * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void APlayerCharacter::LookUpAtRate(float Value)
{
	AddControllerPitchInput(Value * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void APlayerCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	SpringArmComponent->TargetOffset += FVector(0.0f, 0.0f, HalfHeightAdjust);
}

void APlayerCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	SpringArmComponent->TargetOffset -= FVector(0.0f, 0.0f, HalfHeightAdjust);
}

void APlayerCharacter::SwimForward(float Value)
{
	if (GetCharacterMovement()->IsSwimming() && !FMath::IsNearlyZero(Value, 1e-6f))
	{
		FRotator PitchYawRotator(GetControlRotation().Pitch, GetControlRotation().Yaw, 0.0f);
		FVector ForwardVector = PitchYawRotator.RotateVector(FVector::ForwardVector);
		AddMovementInput(ForwardVector, Value);
	}
}

void APlayerCharacter::SwimRight(float Value)
{
	if (GetCharacterMovement()->IsSwimming() && !FMath::IsNearlyZero(Value, 1e-6f))
	{
		FRotator YawRotator(0.0f, GetControlRotation().Yaw, 0.0f);
		FVector RightVector = YawRotator.RotateVector(FVector::RightVector);
		AddMovementInput(RightVector, Value);
	}
}

void APlayerCharacter::SwimUp(float Value)
{
	if (GetCharacterMovement()->IsSwimming() && !FMath::IsNearlyZero(Value, 1e-6f))
	{
		AddMovementInput(FVector::UpVector, Value);
	}
}

void APlayerCharacter::OnStartProne(float HeightAdjust, float ScaledHeightAdjust)
{
	Super::OnStartProne(HeightAdjust, ScaledHeightAdjust);

	SpringArmComponent->TargetOffset += FVector(0.0f, 0.0f, ScaledHeightAdjust);
}

void APlayerCharacter::OnEndProne(float HeightAdjust, float ScaledHeightAdjust)
{
	Super::OnEndProne(HeightAdjust, ScaledHeightAdjust);

	SpringArmComponent->TargetOffset -= FVector(0.0f, 0.0f, ScaledHeightAdjust);
}

void APlayerCharacter::OnStartAimingInternal()
{
	Super::OnStartAimingInternal();
	APlayerController* PlayerController = GetController<APlayerController>();
	if (!IsValid(PlayerController)) 
	{
		return;
	}

	APlayerCameraManager* CameraManager = PlayerController->PlayerCameraManager;
	if (IsValid(CameraManager))
	{
		AimingTimeline.Play();
	}
}

void APlayerCharacter::OnStopAimingInternal()
{
	Super::OnStopAimingInternal();
	APlayerController* PlayerController = GetController<APlayerController>();
	if (!IsValid(PlayerController))
	{
		return;
	}

	APlayerCameraManager* CameraManager = PlayerController->PlayerCameraManager;
	if (IsValid(CameraManager))
	{
		AimingTimeline.Reverse();
	}
}

void APlayerCharacter::AimingTimelineUpdate(float Alpha)
{
	APlayerController* PlayerController = GetController<APlayerController>();
	APlayerCameraManager* CameraManager = PlayerController->PlayerCameraManager;
	ARangeWeaponItem* CurrentRangeWeapon = GetCharacterEquipmentComponent()->GetCurrentRrangeWeapon();
	if (!IsValid(CurrentRangeWeapon))
	{
		CameraManager->SetFOV(CameraManager->GetLockedFOV());
		return;
	}
	float LerpAimingFov = FMath::Lerp(CameraManager->DefaultFOV, CurrentRangeWeapon->GetAimFOV(), Alpha);
	CameraManager->SetFOV(LerpAimingFov);
}
