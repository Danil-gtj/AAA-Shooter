// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GCAtrributeProgressBar.generated.h"

class UGCCharacterAttributeSet;
class UProgressBar;
UCLASS()
class GAMECODE_API UGCAtrributeProgressBar : public UUserWidget
{
	GENERATED_BODY()

public: 
	void SetProgressPercantage(float Percentage);

	void SetAttributeSet(UGCCharacterAttributeSet* AttributeSet_In);

	UFUNCTION(BlueprintCallable)
	float GetHealthPercent() const;

protected:
	UPROPERTY(meta = (BindWidget))
	UProgressBar* ProgressBar;

	TWeakObjectPtr<UGCCharacterAttributeSet> AttributeSet;
};
