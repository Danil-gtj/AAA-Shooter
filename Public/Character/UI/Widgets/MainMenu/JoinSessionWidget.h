// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/UI/Widgets/MainMenu/NetworkWidget.h"
#include "JoinSessionWidget.generated.h"

UENUM(BlueprintType)
enum class ESearchingSessionState : uint8 
{
	None, 
	Searching,
	SessionIsFound
};

class UGCGameInstance;
UCLASS()
class GAMECODE_API UJoinSessionWidget : public UNetworkWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, Transient, BlueprintReadOnly, Category = "NetworkSession")
	ESearchingSessionState SearchinSessionState;

	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable)
	void FindOnlineSession();

	UFUNCTION(BlueprintCallable)
	void JoinOnlineSession();

	UFUNCTION(BlueprintNativeEvent)
	void OnMatchFound(bool bIsSuccesful);

	virtual void CloseWidget() override;

private:
	TWeakObjectPtr<UGCGameInstance> GCGameInstance;

};
