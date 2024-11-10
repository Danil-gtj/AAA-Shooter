// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DroneStatsWidget.generated.h"

UCLASS()
class GAMECODE_API UDroneStatsWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetYReticlePosition(float PosYValue);

	float GetYReticlePosition();

	void SetXReticlePosition(float PosXValue);

	float GetXReticlePosition();

	void SetReticlePositionInViewport();

protected:
	UPROPERTY(meta = (BindWidget))
	UUserWidget* ReticleWidget;

private:
	float PosY;
	float PosX;
};
