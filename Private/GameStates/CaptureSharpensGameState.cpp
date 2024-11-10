#include "Net/UnrealNetwork.h"
#include "GameStates/CaptureSharpensGameState.h"
#include "Multiplayer/Spawns/MultiplayerPlayerStart.h"
#include "Character/GCBaseCharacter.h"
#include "Kismet/GameplayStatics.h"

void ACaptureSharpensGameState::BeginPlay()
{
	Super::BeginPlay();
	Startgame();
}

void ACaptureSharpensGameState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACaptureSharpensGameState, Minutes);
	DOREPLIFETIME(ACaptureSharpensGameState, Seconds);
	DOREPLIFETIME(ACaptureSharpensGameState, PlayersAInZone);
	DOREPLIFETIME(ACaptureSharpensGameState, PlayersBInZone);
}

//~MatchSystem

void ACaptureSharpensGameState::Startgame()
{
	GetWorld()->GetTimerManager().SetTimer(CountdownTimerHandle, this, &ACaptureSharpensGameState::UpdateMatchTimer, 1.0f, true, 0.0f);
}

void ACaptureSharpensGameState::UpdateMatchTimer()
{
	if (Seconds != 0)
	{
		Seconds = Seconds - 1;
	}
	else
	{
		if (Minutes == 0)
		{
			MatchFinish();
		}
		else
		{
			Minutes = Minutes - 1;
			Seconds = 59;
		}
	}
}

void ACaptureSharpensGameState::MatchFinish()
{
	GetWorld()->GetTimerManager().ClearTimer(CountdownTimerHandle);
}

//~MatchSystem

//~Zone System 

void ACaptureSharpensGameState::RegisterlayerToZone(AGCBaseCharacter* InteractivePlayer)
{
	OverlappedPlayersArray.AddUnique(InteractivePlayer);
	if (InteractivePlayer->Team == ETeams::TeamA)
	{
		PlayersAInZone++;
	}
	else if (InteractivePlayer->Team == ETeams::TeamB)
	{
		PlayersBInZone++;
	}
}

void ACaptureSharpensGameState::UnregisterPlayerFromZone(AGCBaseCharacter* InteractivePlayer)
{
	OverlappedPlayersArray.RemoveSingleSwap(InteractivePlayer);
	if (InteractivePlayer->Team == ETeams::TeamA)
	{
		PlayersAInZone--;
	}
	else if (InteractivePlayer->Team == ETeams::TeamB)
	{
		PlayersBInZone--;
	}
}

//~Zone System 

