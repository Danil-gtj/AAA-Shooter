// Copyright Epic Games, Inc. All Rights Reserved.


#include "GameCodeGameModeBase.h"
#include "Character/GCBaseCharacter.h"
#include "Multiplayer/Spawns/MultiplayerPlayerStart.h"
#include "Net/UnrealNetwork.h"
#include "PlayerStates/BasePlayerState.h"
#include "GameCodeTypes.h"
#include "Character/GCPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "GameStates/CaptureSharpensGameState.h"

AGameCodeGameModeBase::AGameCodeGameModeBase()
{
	GameStateClass = ACaptureSharpensGameState::StaticClass();
	PlayerStateClass = ABasePlayerState::StaticClass();
}

void AGameCodeGameModeBase::PlayerRespawn(FName TeamTag, AController* PlayerController, TSubclassOf<AGCBaseCharacter> CahracterToSpawn)
{
	if (!PlayerController)
	{
		return;
	}

	SpawnPlayer(TeamTag, PlayerController);
}

void AGameCodeGameModeBase::SpawnPlayer(FName TeamTag, AController* Controller, TSubclassOf<AGCBaseCharacter> CahracterToSpawn)
{
	TArray<AActor*> FoundSpawns;
	UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), AMultiplayerPlayerStart::StaticClass(), TeamTag, FoundSpawns);

	FTransform PlayerSpawnTrans;

	int32 Index = FMath::RandRange(0, FoundSpawns.Num());

	AActor* Spawn = FoundSpawns[Index];

	AMultiplayerPlayerStart* PlayerStart = Cast<AMultiplayerPlayerStart>(Spawn);
	PlayerSpawnTrans = PlayerStart->GetActorTransform();

	if (AGCBaseCharacter* PawnIn = GetWorld()->SpawnActor<AGCBaseCharacter>(CahracterToSpawn, PlayerSpawnTrans))
	{
		Controller->Possess(PawnIn);
		AGCBaseCharacter* CharacterIn = Cast<AGCBaseCharacter>(PawnIn);
		AGCPlayerController* PlayerController = Cast<AGCPlayerController>(Controller);
		PlayerController->CreateAndInitializeWidtget();
	}
}



