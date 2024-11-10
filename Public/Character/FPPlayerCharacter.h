// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/PlayerCharacter.h"
#include "FPPlayerCharacter.generated.h"

/**
 * 
 */
UCLASS()
class GAMECODE_API AFPPlayerCharacter : public APlayerCharacter
{
	GENERATED_BODY()

public:
	AFPPlayerCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void PossessedBy(AController* NewController);

	virtual void Tick(float DeltaTime) override;

	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	virtual FRotator GetViewRotation() const override;

	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode = 0) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintREadOnly, Category = "Character | FirstPerson")
	class USkeletalMeshComponent* FirstPersonMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintREadOnly, Category = "Character | FirstPerson")
	class UCameraComponent* FirstPersonCameraComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintREadOnly, Category = "Character | FirstPerson | Camera | Ladder | Pitch", meta = (UIMin = -89.0f, UIMax = 89.0f))
	float LadderCameraMinPitch = -60.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintREadOnly, Category = "Character | FirstPerson | Camera | Ladder | Pitch", meta = (UIMin = -89.0f, UIMax = 89.0f))
	float LadderCameraMaxPitch = 80.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintREadOnly, Category = "Character | FirstPerson | Camera | Ladder | Yaw", meta = (UIMin = 0.0f, UIMax = 359.0f))
	float LadderCameraMinYaw = 5.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintREadOnly, Category = "Character | FirstPerson | Camera | Ladder | Yaw", meta = (UIMin = 0.0f, UIMax = 359.0f))
	float LadderCameraMaxYaw = 175.0f;


	UPROPERTY(EditDefaultsOnly, BlueprintREadOnly, Category = "Character | FirstPerson | Camera | Zipline | Pitch", meta = (UIMin = -89.0f, UIMax = 89.0f))
	float ZiplineCameraMinPitch = -60.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintREadOnly, Category = "Character | FirstPerson | Camera | Zipline | Pitch", meta = (UIMin = -89.0f, UIMax = 89.0f))
	float ZiplineCameraMaxPitch = 60.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintREadOnly, Category = "Character | FirstPerson | Camera | Zipline | Pitch", meta = (UIMin = -89.0f, UIMax = 89.0f))
	float ZiplineCameraMinYaw = -89.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintREadOnly, Category = "Character | FirstPerson | Camera | Zipline | Pitch", meta = (UIMin = -89.0f, UIMax = 89.0f))
	float ZiplineCameraMaxYaw = 89.0f;

	virtual void OnMantle(const FMantlingSettings& MantlingSettings, float MantlingAnimationStartTime) override;

private:

	FTimerHandle MontageFPTimer;

	void OnFPMontageTimerElapsed();

	bool IsFPMontagePlaying() const;

	TWeakObjectPtr<class AGCPlayerController> GCPlayerController;
};
