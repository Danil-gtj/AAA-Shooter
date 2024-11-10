// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/GCGameInstance.h"
#include "Net/UnrealNetwork.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "OnlineSubsystemTypes.h"
#include "Kismet/GameplayStatics.h"

UGCGameInstance::UGCGameInstance()
{
	OnCreateSessionCompleteDelegate = FOnCreateSessionCompleteDelegate::CreateUObject(this, &UGCGameInstance::OnCreateSessionComplete);
	OnStartSessionCompleteDelegate = FOnStartSessionCompleteDelegate::CreateUObject(this, &UGCGameInstance::OnStartOnlineGameComplete);

	OnFindSessionsCompleteDelegate = FOnFindSessionsCompleteDelegate::CreateUObject(this, &UGCGameInstance::OnFindSessionsComplete);

	OnJoinSessionComplereDelegate = FOnJoinSessionCompleteDelegate::CreateUObject(this, &UGCGameInstance::OnJoinSessionComplete);

	OnDestroySessionCompleteDelegate = FOnDestroySessionCompleteDelegate::CreateUObject(this, &UGCGameInstance::OnDestroySessionComplete);
}

void UGCGameInstance::LaunchLobby(uint32 MaxPlayers_In, FName ServerName_In, bool bIsLAN)
{
	MaxPlayers = MaxPlayers_In;
	ServerName = ServerName_In;
	HostSession(GetPrimaryPlayerUniqueId(), ServerName, bIsLAN, true, MaxPlayers);
}

void UGCGameInstance::FindMatch(bool bIsLAN)
{
	FindSessions(GetPrimaryPlayerUniqueId(), bIsLAN, true);
}

void UGCGameInstance::JoinOnlineGame()
{
	FOnlineSessionSearchResult SearchResult;

	TSharedPtr<const FUniqueNetId> PlayerUniqueNetId = GetPrimaryPlayerUniqueId();

	if (SessionSearch->SearchResults.Num() > 0)
	{
		for (int32 i = 0; i < SessionSearch->SearchResults.Num(); i++) 
		{
			if(SessionSearch->SearchResults[i].Session.OwningUserId != PlayerUniqueNetId)
			{
				SearchResult = SessionSearch->SearchResults[i];

				if (!JoinFoundOnlineSession(PlayerUniqueNetId, GameSessionName, SearchResult)) 
				{
					DisplayNetworkErrorMessage("Failed to join a session! Please try again!");
				}
				break;
			}
		}
	}
}

void UGCGameInstance::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UGCGameInstance, ServerName);
	DOREPLIFETIME(UGCGameInstance, MaxPlayers);
}

void UGCGameInstance::Init()
{
	Super::Init();
	/*OnNetworkFailureEventHandle = GEngine->NetworkFailureEvent.AddUFunction(this, "OnNetworkFailure");
	OnTravelFailureEventHandle = GEngine->TravelFailureEvent.AddUFunction(this, "OnTravelFailure");*/
}

void UGCGameInstance::Shutdown()
{
	Super::Shutdown();
	/*GEngine->NetworkFailureEvent.Remove(OnNetworkFailureEventHandle);
	GEngine->TravelFailureEvent.Remove(OnTravelFailureEventHandle);*/
}

//bool UGCGameInstance::ProcessConsoleExec(const TCHAR* Cmd, FOutputDevice& Ar, UObject* Executoe)
//{
//	return false;
//}

bool UGCGameInstance::HostSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, bool bIsLAN, bool bIsPresence, int32 MaxNumPlayers)
{
	IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();

	if (OnlineSub) 
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

		if (Sessions.IsValid() && UserId.IsValid()) 
		{
			SessionSettings = MakeShareable(new FOnlineSessionSettings());

			SessionSettings->bIsLANMatch = bIsLAN;
			SessionSettings->bUsesPresence = bIsPresence;
			SessionSettings->NumPublicConnections = MaxNumPlayers;
			SessionSettings->NumPrivateConnections = 0;
			SessionSettings->bAllowInvites = true;
			SessionSettings->bAllowJoinInProgress = true;
			SessionSettings->bShouldAdvertise = true;
			SessionSettings->bAllowJoinViaPresence = true;
			SessionSettings->bAllowJoinViaPresenceFriendsOnly = false;

			SessionSettings->Set(SETTING_MAPNAME, LobbyMapName.ToString(), EOnlineDataAdvertisementType::ViaOnlineService);

			OnCreateSessionCompleteDelegateHandle = Sessions->AddOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegate);

			return Sessions->CreateSession(*UserId, SessionName, *SessionSettings);
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("No OnlineSubsystem found!"));
	}

	return false;
}

void UGCGameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("OnCreateSessionComplete")));

	if (!bWasSuccessful) 
	{
		DisplayNetworkErrorMessage("Failed to create session! Please try again");
		return;
	}

	IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			Sessions->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegateHandle);
			if (bWasSuccessful)
			{
				OnStartSessionCompliteDelegateHandle = Sessions->AddOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegate);

				Sessions->StartSession(SessionName);
			}
		}
	}
}

void UGCGameInstance::OnStartOnlineGameComplete(FName SeesionName, bool bWasSuccessful)
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("OnCreateSessionComplete")));

	if (!bWasSuccessful)
	{
		DisplayNetworkErrorMessage("Failed to create session! Please try again");
		return;
	}

	IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			Sessions->ClearOnStartSessionCompleteDelegate_Handle(OnStartSessionCompliteDelegateHandle);
		}
	}

	UGameplayStatics::OpenLevel(GetWorld(), LobbyMapName, true, "listen");
}

void UGCGameInstance::FindSessions(TSharedPtr<const FUniqueNetId> UserId, bool bIsLan, bool bIsPresence)
{
	IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub) 
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

		if (Sessions.IsValid() && UserId.IsValid())
		{
			SessionSearch = MakeShareable(new FOnlineSessionSearch());

			SessionSearch->bIsLanQuery = bIsLan;
			SessionSearch->MaxSearchResults = 20;
			SessionSearch->PingBucketSize = 50;
			 
			if (bIsPresence) 
			{
				SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, bIsPresence, EOnlineComparisonOp::Equals);
			}

			TSharedRef<FOnlineSessionSearch> SearchSettingsRef = SessionSearch.ToSharedRef();

			OnFindSessionsCompleteDelegateHandle = Sessions->AddOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegate);

			Sessions->FindSessions(*UserId, SearchSettingsRef);
		}
	}
	else
	{
		OnFindSessionsComplete(false);
	}
}

void UGCGameInstance::OnFindSessionsComplete(bool bWasSuccessful)
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("OnFindingSessionComplete")));

	if (!bWasSuccessful)
	{
		DisplayNetworkErrorMessage("Failed to find session! Please try again");
		return;
	}

	bool bIsMatchfound = false;

	IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub) 
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			Sessions->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegateHandle);

			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Num Search Results")));

			if (SessionSearch->SearchResults.Num() > 0)
			{
				bIsMatchfound = true;
				for (int32 SearchIdx = 0; SearchIdx < SessionSearch->SearchResults.Num(); SearchIdx++) 
				{
					GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Session Number: ")));
				}
			}
		}
	}

	if (OnMatchFound.IsBound()) 
	{
		OnMatchFound.Broadcast(bIsMatchfound);
	}
}

bool UGCGameInstance::JoinFoundOnlineSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, const FOnlineSessionSearchResult& SearchResult)
{
	bool bSuccsessful = false;

	IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid() && UserId.IsValid())
		{
			OnJoinSessionCompleteDelegateHandle = Sessions->AddOnJoinSessionCompleteDelegate_Handle(OnJoinSessionComplereDelegate);

			bSuccsessful = Sessions->JoinSession(*UserId, SessionName, SearchResult);
		}
	}

	return bSuccsessful;
}

void UGCGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("OnJoinSessionComplete")));

	IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			Sessions->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegateHandle);

			APlayerController* const PlayerController = GetFirstLocalPlayerController();

			FString TravelURL;

			if (IsValid(PlayerController) && Sessions->GetResolvedConnectString(SessionName, TravelURL))
			{
				PlayerController->ClientTravel(TravelURL, ETravelType::TRAVEL_Absolute);
			}
		}
	}
}

void UGCGameInstance::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("OnJoinSessionComplete")));

	IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			Sessions->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegateHandle);

			if (bWasSuccessful) 
			{
				UGameplayStatics::OpenLevel(GetWorld(), MainMenuName, true);
			}
		}
	}
}

//void UGCGameInstance::OnNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorMessage)
//{
//	DisplayNetworkErrorMessage(ErrorMessage);
//}
//
//void UGCGameInstance::OnTravelFailure(UWorld* World, ENetworkFailure::Type FailureType, const FString& ErrorMessage)
//{
//	DisplayNetworkErrorMessage(ErrorMessage);
//}

void UGCGameInstance::DisplayNetworkErrorMessage(FString ErrorMessage)
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, ErrorMessage);
}
