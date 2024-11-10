// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WidgetCharacterAttributes.generated.h"

/**
 * 
 */
UCLASS()
class GAMECODE_API UWidgetCharacterAttributes : public UUserWidget
{
	GENERATED_BODY()

protected:
	UFUNCTION(BlueprintCallable)
	float GetStaminaPercent() const;

	UFUNCTION(BlueprintCallable)
	float GetOxygenPercent() const;

	UFUNCTION(BlueprintCallable)
	float GetHealthPercent() const;
};
