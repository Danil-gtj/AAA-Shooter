// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/UI/Widgets/TeamSelect/TeamSelectWidget.h"
#include "GameCode/GameCodeGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Character/GCPlayerController.h"
#include "Character/GCBaseCharacter.h"
#include "GameStates/CaptureSharpensGameState.h"
#include "Actors/GCGameInstance.h"
#include "PlayerStates/BasePlayerState.h"

void UTeamSelectWidget::SelectTeam(FName TeamTag, TSubclassOf<AGCBaseCharacter> CahracterToChoose)
{
	//AGameCodeGameModeBase* GameMode = Cast<AGameCodeGameModeBase>(GetWorld()->GetAuthGameMode());
	AGCPlayerController* Controller = Cast<AGCPlayerController>(GetOwningPlayer());
	ACaptureSharpensGameState* GamseState = Cast<ACaptureSharpensGameState>(UGameplayStatics::GetGameState(GetWorld()));

	Controller->SpawnPlayer(TeamTag, CahracterToChoose);
	GamseState->Startgame();
	RemoveFromParent();
}
