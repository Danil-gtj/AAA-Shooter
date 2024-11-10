// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/UI/Widgets/MainMenu/NetworkWidget.h"
#include "HostSessionWidget.generated.h"

/**
 * 
 */
UCLASS()
class GAMECODE_API UHostSessionWidget : public UNetworkWidget
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NetworkSession")
	FName ServerName;

	UFUNCTION(BlueprintCallable)
	void CreateSession();
};
