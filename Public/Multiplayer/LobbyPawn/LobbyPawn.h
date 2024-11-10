// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "LobbyPawn.generated.h"

UCLASS()
class GAMECODE_API ALobbyPawn : public APawn
{
	GENERATED_BODY()

public:
	ALobbyPawn();

protected:
	virtual void BeginPlay() override;
};
