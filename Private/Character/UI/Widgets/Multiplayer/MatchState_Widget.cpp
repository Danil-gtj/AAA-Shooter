// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/UI/Widgets/Multiplayer/MatchState_Widget.h"
#include "Kismet/GameplayStatics.h"
#include "GameStates/CaptureSharpensGameState.h"

FString UMatchState_Widget::GetMatchSeconds()
{
	ACaptureSharpensGameState* GameState = Cast<ACaptureSharpensGameState>(UGameplayStatics::GetGameState(GetWorld()));
	int Seconds = GameState->Seconds;
	FString StrSeconds;

	if (Seconds < 10)
	{
		StrSeconds = TEXT("0" + FString::FromInt(Seconds));
	}
	else
	{
		StrSeconds = FString::FromInt(Seconds);
	}
	return StrSeconds;
}

FString UMatchState_Widget::GetMatchMinutes()
{
	ACaptureSharpensGameState* GameState = Cast<ACaptureSharpensGameState>(UGameplayStatics::GetGameState(GetWorld()));
	int Minutes = GameState->Minutes;
	FString StrMinutes;

	if (Minutes < 10)
	{
		StrMinutes = TEXT("0" + FString::FromInt(Minutes));
	}
	else
	{
		StrMinutes = FString::FromInt(Minutes);
	}
	return StrMinutes;
}
