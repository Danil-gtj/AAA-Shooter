// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MatchState_Widget.generated.h"

/**
 * 
 */
UCLASS()
class GAMECODE_API UMatchState_Widget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	FString GetMatchSeconds();

	UFUNCTION(BlueprintCallable)
	FString GetMatchMinutes();
};
