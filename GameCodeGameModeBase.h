// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameCodeGameModeBase.generated.h"

class AMultiplayerPlayerStart;
class AGCBaseCharacter;
class ACaptureSharpensGameState;
class APawn;
UCLASS()
class GAMECODE_API AGameCodeGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	AGameCodeGameModeBase();

	UFUNCTION()
	void PlayerRespawn(FName TeamTag, AController* Controller, TSubclassOf<AGCBaseCharacter> CahracterToSpawn = nullptr);

	void SpawnPlayer(FName TeamTag, AController* Controller, TSubclassOf<AGCBaseCharacter> CahracterToSpawn = nullptr);

	UPROPERTY(EditAnywhere)
	TSubclassOf<AGCBaseCharacter> CachedCharacter;
};
