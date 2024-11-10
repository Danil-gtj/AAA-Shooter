// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Character/LedgeDetectorComponent.h"
#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Character/GCBaseCharacter.h"
#include "GCBaseCharacterMovementComponent.generated.h"

struct FMantlingMovementParameters
{
	FVector InitialLocation = FVector::ZeroVector;
	FRotator InitialRotation = FRotator::ZeroRotator;

	FVector TargetLocation = FVector::ZeroVector;
	FRotator TargetRotation = FRotator::ZeroRotator;

	FVector InitialAnimationLocation = FVector::ZeroVector;

	float Duration = 1.0f;
	float StartTime = 0.0f;

	UCurveVector* MantlingCurve;

	UPROPERTY() 
	UPrimitiveComponent* Component = nullptr;
};

UENUM(BlueprintType)
enum class ECustomMovementMode : uint8
{
	CMOVE_None = 0 UMETA(DisplayName = "None"),
	CMOVE_Mantling UMETA(DisplayName = "Mantling"),
	CMOVE_OnLadder UMETA(DisplayName = "Ledder"),
	CMOVE_Zipline UMETA(DisplayName = "Zipline"),
	CMOVE_Max  UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EDetachFromLadderMethod : uint8
{
	Fall = 0,
	ReachingTheTop,
	ReachingTheBottom,
	JumpOff
};

UCLASS()
class GAMECODE_API UGCBaseCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

	friend class FSavedMove_GC;

public:
	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;

	virtual void UpdateFromCompressedFlags(uint8 Flags) override;

	virtual void PhysicsRotation(float DeltaTime) override;

	FORCEINLINE bool IsSprinting() { return bIsSprinting; }

	FORCEINLINE bool IsOutOfStamina() { return bIsOutOfStamina; }

	virtual float GetMaxSpeed() const override;

	void StartSprint();
	void StopSprint();

	void AttachToZipline(const class AZipline* Ladder);

	void StartMantle(const FMantlingMovementParameters& MantlingParameters);
	void EndMantle();
	bool IsMantling() const;

	bool IsOnZipline();
	void DetachFromZipline();

	void AttachToLadder(const class ALadder* Ladder);
	void DetachFromladder(EDetachFromLadderMethod EDetachFromLadderMethod);
	bool IsOnLadder() const;
	const class ALadder* GetCurrenLadder();

	float GetLadderSpeedRatio() const;

	void SetIsOutOfStamina(bool bIsOutOfStamina_In);

	virtual bool IsProning() const;
	virtual void Prone(bool bClientSimulation = false);
	virtual void UnProne(bool bClientSimulation = false);
	virtual void UpdateCharacterStateBeforeMovement(float DeltaSeconds) override;
	virtual bool CanProneInCurrentState() const;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Character | Prone", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	bool bWantsToProne = false;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Character | Prone", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	bool bIsFullHeight = false;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Character | Prone", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	bool bProneMaintainsBaseLocation = false;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Character | Prone", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float PronedHalfHeight = false;

protected:
	virtual void PhysCustom(float DeltaTime, int32 Iterations) override;

	void PhysMantling(float DeltaTime, int32 Iterations);
	void PhysLadder(float DeltaTime, int32 Iterations);
	void PhysZipline(float DeltaTime, int32 Iteraions);

	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement: Sprint", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float SprintSpeed = 800.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character movement: Swimming", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float SwimmingCapsuleRadius = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement: Swimming", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float SwimmingCapsuleHalfHeight = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement: Ladder", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float ClimbingOnLadderSpeed = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement: Ladder", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float ClimbingOnLadderBrealingDeceleration = 2048.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement: Ladder", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float LadderToCharacterOffset = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement: Ladder", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float MaxLadderTopOffset = 90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement: Ladder", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float MinLadderBottomOffset = 90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement: Ladder", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float JumpOffFromLadderSpeed = 400.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement: Zipline", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float SpeedOnZipline = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement: Prone", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float ProneCapsuleRadius = 40.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement: Prone", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float ProneCapsuleHalfHeight = 40.0f;

	class AGCBaseCharacter* GetBaseCharacterOwner() const;
private:
	bool bIsSprinting = false;
	bool bIsOutOfStamina = false;

	FMantlingMovementParameters CurrentMantlingParameters;

	FTimerHandle MantlingTimer;
	float TargetMantlingTime = 1.0f;

	UPROPERTY()
	const ALadder* CurrentLadder = nullptr;

	UPROPERTY()
	const AZipline* CurrentZipline = nullptr;

	float GetCharacterToCurrentLadderProjection(const FVector& Location);

	FRotator ForcedTargetRotation = FRotator::ZeroRotator;

	bool bForceRotation = false;
};

class FSavedMove_GC : public FSavedMove_Character
{
	typedef FSavedMove_Character Super;

public:
	virtual void Clear() override;

	virtual uint8 GetCompressedFlags() const;

	virtual bool CanCombineWith(const FSavedMovePtr& NewMovePtr, ACharacter* InCharacter, float MaxDelta) const override;

	virtual void SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character & ClientData) override;

	virtual void PrepMoveFor(ACharacter* Character) override;
private:
	uint8 bSavedIsSprinting : 1;
	uint8 bSavedIsMantling : 1;
};

class FNetworkPredictionData_Client_Character_GC : public FNetworkPredictionData_Client_Character 
{
	typedef FNetworkPredictionData_Client_Character Super;

public:
	FNetworkPredictionData_Client_Character_GC(const UCharacterMovementComponent& ClientMovement);

	virtual FSavedMovePtr AllocateNewMove() override;
};