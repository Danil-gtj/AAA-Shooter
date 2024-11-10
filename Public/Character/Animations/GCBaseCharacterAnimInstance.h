// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GameCode/GameCodeTypes.h"
#include "GCBaseCharacterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class GAMECODE_API UGCBaseCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animtaion", meta = (UIMin = 0.0f, UIMax = 500.0f))
	float Speed = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animtaion")
	bool bIsFalling = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animtaion")
	bool bIsCrouching = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animtaion")
	bool bIsSprinting = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animtaion")
	bool bIsSwimming = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animtaion")
	bool bIsoutOfStamina = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animtaion")
	bool bIsClimbingOnLadder = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animtaion")
	float LadderSpeedRatio = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animtaion")
	bool bIsStrafing = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animtaion", meta = (UIMin = -180, UIMax = 180))
	float Direction = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animtaion", meta = (UIMin = -90, UIMax = 90))
	FRotator AimRotation = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animtaion")
	bool bIsProne = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animtaion")
	bool bOnZipline = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animtaion")
	EEquipableItemType CurrentEquippedItem = EEquipableItemType::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animtaion | Weapon")
	FTransform ForeGripSocketTransform;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character animtaion | Weapon")
	bool bIsAiming;

	UPROPERTY(VisibleAnywhere, Transient, BlueprintReadOnly, Category = "Character | IK")
	FVector IKRightFootOffset = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, Transient, BlueprintReadOnly, Category = "Character | IK")
	FVector IKLeftFootOffset = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, Transient, BlueprintReadOnly, Category = "Character | IK")
	FVector IKPelvisBoneOffset = FVector::ZeroVector;

private:
	TWeakObjectPtr<class AGCBaseCharacter> CachedBaseCharacter;
};
