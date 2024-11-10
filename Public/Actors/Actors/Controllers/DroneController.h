// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "DroneController.generated.h"

UCLASS()
class GAMECODE_API ADroneController : public APlayerController
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;
	
	virtual void SetOwner(AActor* NewOwner) override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void SetupInputComponent() override;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void TurnAtRate(float Value);
	void LookUpAtRate(float Value);
	void MoveUp(float Value);

	void NextDrone();
	void PreviousDrone();

	void ReturnToPlayerControll();

	void SelfDestruction();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget")
	TSubclassOf<class UDroneStatsWidget> DroneHUDWidgetClass;

private:
	void CreateWidgetInit();

	UDroneStatsWidget* DroneIntefaceWidget = nullptr;

	TWeakObjectPtr<class AGCDroneCharacter> CachedBaseDrone;
};
