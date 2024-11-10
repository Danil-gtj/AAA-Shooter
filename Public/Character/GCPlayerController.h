// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GCPlayerController.generated.h"

class AGCBaseCharacter;
class UTeamSelectWidget;
class UPlayerHUDWidget;
UCLASS()
class GAMECODE_API AGCPlayerController : public APlayerController
{
	GENERATED_BODY()

public: 
	virtual void BeginPlay() override;

	virtual void SetPawn(APawn* InPawn) override;

	bool GetIgnoreCameraPitch() const;

	void SetIgnoreCameraPitch(bool bIgnoreCameraPitch_In);

	virtual void Tick(float DeltaSenonds) override;

	UPlayerHUDWidget* GetPlayerHUDWidget();

	void CreateAndInitializeWidtget();

	void ToggleMainMenu();

	void SpawnPlayer(FName TeamTag, TSubclassOf<AGCBaseCharacter> CahracterToSpawn = nullptr);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget")
	TSubclassOf<class UPlayerHUDWidget> PlayerHUDWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget")
	TSubclassOf<class UOptionsMenuWidget> OptionsWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget")
	TSubclassOf<class UTeamSelectWidget> TeamSelectWidgetClass;

	virtual void SetupInputComponent() override;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void TurnAtRate(float Value);
	void LookUpAtRate(float Value);
	void ChangeCrouchState();
	void Jump();
	void ChangeProneState();

	void Mantle();
	void ClimbLadderUp(float Value);
	void InteractWithLadder();
	void InteractWirhZipline();

	void SwimForward(float Value);
	void SwimRight(float Value);
	void SwimUp(float Value);

	void StartSprint();
	void StopSprint();

	void BeginFire();
	void FinishFire();

	void StartAiming();
	void StopAiming();

	void Reload();

	void NextItem();
	void PreviousItem();

	void PrimaryMeleeAttack();
	void SecondaryMeleeAttack();

	void EquipPrimaryItem();

	void LaunchDrone();

	void ConnectToDrone();

	void CreateChangeTeamWidget();

	TWeakObjectPtr<class AGCBaseCharacter> CachedBaseCharacter;

private:
	UTeamSelectWidget* TeamSelectWidget = nullptr;

	UPlayerHUDWidget* PlayerHUDWidget = nullptr;

	UOptionsMenuWidget* OptionsWidget = nullptr;

	bool bIgnoreCameraPitch = false;
};
