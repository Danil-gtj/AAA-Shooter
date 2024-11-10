// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NicknameWidget.generated.h"

/**
 * 
 */
UCLASS()
class GAMECODE_API UNicknameWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void SetPlayerName(FText PlayerNick);

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UTextBlock* PlayerNickText;
};
