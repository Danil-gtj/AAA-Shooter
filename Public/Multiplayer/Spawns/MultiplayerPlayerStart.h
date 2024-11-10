// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameCode/GameCodeTypes.h"
#include "GameFramework/PlayerStart.h"
#include "MultiplayerPlayerStart.generated.h"

UCLASS()
class GAMECODE_API AMultiplayerPlayerStart : public APlayerStart
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Team")	
	ETeams Team;
};
