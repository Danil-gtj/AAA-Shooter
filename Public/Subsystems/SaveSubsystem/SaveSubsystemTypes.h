// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "SaveData.h"
#include "SaveSubsystemTypes.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogSaveSubsystem, Log, All);

const FName FileExtensionSave = TEXT("save");

struct FSaveSubsystemArchive : public FObjectAndNameAsStringProxyArchive
{
	FSaveSubsystemArchive(FArchive& InInnerArchive, bool bInLoadIfFindFails);
};

class FSaveDirectoryVisitor : public IPlatformFile::FDirectoryVisitor
{
public:
	FSaveDirectoryVisitor(TArray<int32>& InSavesIds);
	virtual bool Visit(const TCHAR* FilenameOrDirectory, bool bIsDirectory) override;

private:
	TArray<int32>& SaveIds;
};

struct BoolScopeWrapper
{

public:
	BoolScopeWrapper(bool& bInValue, bool bNewValue);
	~BoolScopeWrapper();

private:
	bool& bValue;
	bool bInitialValue;
};

class USaveSubsystem;
class ULevelStreaming;
UCLASS()
class UStreamingLevelObserver : public UObject
{
	GENERATED_BODY()

public:
	UStreamingLevelObserver();

	void Initialize(USaveSubsystem* InSaveSubsystem, ULevelStreaming* InStreamingLevel);
	void Deinitialize();

	virtual void Serialize(FArchive& Archive) override;

private:
	UFUNCTION()
	void OnLevelShown();

	UFUNCTION()
	void OnLevelHidden();

	FLevelSaveData LevelSaveData;
	TWeakObjectPtr<USaveSubsystem> SaveSubsystem;
	TWeakObjectPtr<ULevelStreaming> StreamingLevel;
};
