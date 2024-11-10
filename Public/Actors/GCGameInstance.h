// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "GCGameInstance.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnMatchFound, bool);

class AMultiplayerPlayerStart;
UCLASS()
class GAMECODE_API UGCGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UGCGameInstance();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	//virtual bool ProcessConsoleExec(const TCHAR* Cmd, FOutputDevice& Ar, UObject* Executoe) override;
	FOnMatchFound OnMatchFound;

	virtual void Init() override;

	virtual void Shutdown() override;

	void LaunchLobby(uint32 MaxPlayers_In, FName ServerName_In, bool bIsLAN);

	void FindMatch(bool bIsLAN);

	void JoinOnlineGame();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FText PlayerNickname;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName LobbyMapName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName MainMenuName;

protected:
	UPROPERTY(Replicated, EditAnywhere, Category = "Server settings")
	FName ServerName;

	UPROPERTY(Replicated, EditAnywhere, Category = "Server settings")
	uint32 MaxPlayers;

#pragma region CreatingNetworkSession
	bool HostSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, bool bIsLAN, bool bIsPresence, int32 MaxNumPlayers);

	FOnCreateSessionCompleteDelegate OnCreateSessionCompleteDelegate;
	FOnStartSessionCompleteDelegate OnStartSessionCompleteDelegate;

	TSharedPtr<class FOnlineSessionSettings> SessionSettings;

	FDelegateHandle OnCreateSessionCompleteDelegateHandle;
	FDelegateHandle OnStartSessionCompliteDelegateHandle;

	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);

	void OnStartOnlineGameComplete(FName SeesionName, bool bWasSuccessful);

#pragma endregion CreatingNetworkSession

#pragma region FingingNetworkSessions

	void FindSessions(TSharedPtr<const FUniqueNetId> UserId, bool bIsLan, bool bIsPresence);

	FOnFindSessionsCompleteDelegate OnFindSessionsCompleteDelegate;

	FDelegateHandle OnFindSessionsCompleteDelegateHandle;

	TSharedPtr<class FOnlineSessionSearch> SessionSearch;

	void OnFindSessionsComplete(bool bWasSuccessful);

#pragma endregion FingingNetworkSessions

#pragma region JoiningNetworkSessions

	bool JoinFoundOnlineSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, const FOnlineSessionSearchResult& SearchResult);

	FOnJoinSessionCompleteDelegate OnJoinSessionComplereDelegate;

	FDelegateHandle OnJoinSessionCompleteDelegateHandle;

	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

#pragma endregion JoiningNetworkSessions

#pragma region DestroyingASession

	FOnDestroySessionCompleteDelegate OnDestroySessionCompleteDelegate;

	FDelegateHandle OnDestroySessionCompleteDelegateHandle;

	virtual void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);

#pragma endregion DestroyingASession

#pragma region HandlingNetworkErrors

	FDelegateHandle OnNetworkFailureEventHandle;
	
	FDelegateHandle OnTravelFailureEventHandle;

	/*UFUNCTION()
	void OnNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorMessage);

	UFUNCTION()
	void OnTravelFailure(UWorld* World, ENetworkFailure::Type FailureType, const FString& ErrorMessage);*/

#pragma endregion HandlingNetworkErrors

private:
	void DisplayNetworkErrorMessage(FString ErrorMessage);
};
