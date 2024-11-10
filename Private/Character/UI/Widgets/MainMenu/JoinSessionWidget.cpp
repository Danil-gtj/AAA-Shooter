// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/UI/Widgets/MainMenu/JoinSessionWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Actors/GCGameInstance.h"

void UJoinSessionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(GetWorld());
	check(GameInstance->IsA<UGCGameInstance>());
	GCGameInstance = StaticCast<UGCGameInstance*>(GetGameInstance());
}

void UJoinSessionWidget::FindOnlineSession()
{
	GCGameInstance->OnMatchFound.AddUFunction(this, FName("OnMatchFound"));
	GCGameInstance->FindMatch(bIsLAN);
	SearchinSessionState = ESearchingSessionState::Searching;
}

void UJoinSessionWidget::JoinOnlineSession()
{
	GCGameInstance->JoinOnlineGame();
}

void UJoinSessionWidget::OnMatchFound_Implementation(bool bIsSuccesful)
{
	SearchinSessionState = bIsSuccesful ? ESearchingSessionState::SessionIsFound : ESearchingSessionState::None;
	GCGameInstance->OnMatchFound.RemoveAll(this);
}

void UJoinSessionWidget::CloseWidget()
{
	Super::CloseWidget();
	GCGameInstance->OnMatchFound.RemoveAll(this);
}


