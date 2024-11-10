// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "CaptureSharpensGameState.generated.h"

class AGCBaseCharacter;
class AMultiplayerPlayerStart;
UCLASS()
class GAMECODE_API ACaptureSharpensGameState : public AGameStateBase
{
	GENERATED_BODY()

public:

	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	//Match System

	void Startgame();

	UPROPERTY(Replicated, EditAnywhere, Category = "Match settings");
	int Minutes;

	UPROPERTY(Replicated, EditAnywhere, Category = "Match settings")
	int Seconds;

	UPROPERTY(EditAnywhere, Category = "Match settings")
	bool bStartGame = false;

	void UpdateMatchTimer();

	void MatchFinish();

	FTimerHandle CountdownTimerHandle;

	//Match System

	//Capture Zone

	TArray<AGCBaseCharacter*> OverlappedPlayersArray;

	UPROPERTY(Replicated)
	int32 PlayersBInZone;

	UPROPERTY(Replicated)
	int32 PlayersAInZone;

	void RegisterlayerToZone(AGCBaseCharacter* InteractivePlayer);

	void UnregisterPlayerFromZone(AGCBaseCharacter* InteractivePlayer);

	//Capture Zone
};
