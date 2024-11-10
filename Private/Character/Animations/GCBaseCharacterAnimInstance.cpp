

#include "Character/Animations/GCBaseCharacterAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Character/GCBaseCharacter.h"
#include "Character/MovementComponents/GCBaseCharacterMovementComponent.h"
#include "Character/CharacterComponents/CharacterEquipmentComponent.h"

void UGCBaseCharacterAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	checkf(TryGetPawnOwner()->IsA<AGCBaseCharacter>(), TEXT("UGCBaseCharacterAnimInstance::NativeBeginPlay() can use only GCBaseCaharacter"));
	CachedBaseCharacter = StaticCast<AGCBaseCharacter*>(TryGetPawnOwner());
}

void UGCBaseCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if (!CachedBaseCharacter.IsValid())
	{
		return;
	}

	IKRightFootOffset = FVector((CachedBaseCharacter->GetIKRightFootOffset() + CachedBaseCharacter->CalculateIKPelvisOffset()), 0.0f, 0.0f);
	IKLeftFootOffset = FVector(-(CachedBaseCharacter->GetIKLeftFootOffset() + CachedBaseCharacter->CalculateIKPelvisOffset()), 0.0f, 0.0f);
	IKPelvisBoneOffset = FVector(0.0f, 0.0f, CachedBaseCharacter->CalculateIKPelvisOffset());

	bIsAiming = CachedBaseCharacter->IsAiming();

	UGCBaseCharacterMovementComponent* CharacterMovement = CachedBaseCharacter->GetBaseCharacterMovementComponent();
	Speed = CharacterMovement->Velocity.Size();
	bIsFalling = CharacterMovement->IsFalling();
	bIsCrouching = CharacterMovement->IsCrouching();
	bIsSprinting = CharacterMovement->IsSprinting();
	bIsSwimming = CharacterMovement->IsSwimming();
	bIsoutOfStamina = CharacterMovement->IsOutOfStamina();
	bIsClimbingOnLadder = CharacterMovement->IsOnLadder();
	if (bIsClimbingOnLadder)
	{
		LadderSpeedRatio = CharacterMovement->GetLadderSpeedRatio();
	}
	bIsStrafing = !CharacterMovement->bOrientRotationToMovement;
	bIsProne = CharacterMovement->IsProning();
	Direction = CalculateDirection(CharacterMovement->Velocity, CachedBaseCharacter->GetActorRotation());
	bOnZipline = CharacterMovement->IsOnZipline();

	AimRotation = CachedBaseCharacter->GetAimOffset();

	const UCharacterEquipmentComponent* CharacterEquipment = CachedBaseCharacter->GetCharacterEquipmentComponent();
	CurrentEquippedItem = CharacterEquipment->GetCurrentEquippedItemType();

	ARangeWeaponItem* CurrentRangeWeapon = CharacterEquipment->GetCurrentRrangeWeapon();
	if (IsValid(CurrentRangeWeapon))
	{
		ForeGripSocketTransform = CurrentRangeWeapon->GetForeGripTransform();
	}
}
