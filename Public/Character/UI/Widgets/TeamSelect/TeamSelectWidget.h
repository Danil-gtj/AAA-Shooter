// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameCode/GameCodeTypes.h"
#include "Blueprint/UserWidget.h"
#include "TeamSelectWidget.generated.h"

class AGCBaseCharacter;
UCLASS()
class GAMECODE_API UTeamSelectWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void SelectTeam(FName TeamTag, TSubclassOf<AGCBaseCharacter> CahracterToChoose);
};
