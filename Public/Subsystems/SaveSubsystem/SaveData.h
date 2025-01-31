#pragma once

#include "CoreMinimal.h"
#include "UObject/StrongObjectPtr.h"
#include "SaveData.generated.h"

/* ONLY_SaveDataStruct*/

USTRUCT()
struct FBaseSaveData
{
	GENERATED_BODY()

public:
	FBaseSaveData();

	virtual ~FBaseSaveData() {}
	virtual bool Serialize(FArchive& Archive);

	friend FArchive& operator << (FArchive& Archive, FBaseSaveData& SaveData)
	{
		SaveData.Serialize(Archive);
		return Archive;
	}

	FName Name;
};

FORCEINLINE bool operator == (const FBaseSaveData& First, const FBaseSaveData& Second) { return First.Name == Second.Name; }

/* ObjectSaveDataStruct*/

USTRUCT()
struct FObjectSaveData : public FBaseSaveData
{
	GENERATED_BODY()

public:
	FObjectSaveData();
	FObjectSaveData(const UObject* Object);

	virtual bool Serialize(FArchive& Archive) override;

	TStrongObjectPtr<UClass> Class;
	TArray<uint8> RawData;
};

/* ActorSaveData */

USTRUCT()
struct FActorSaveData : public FObjectSaveData
{
	GENERATED_BODY()

	FActorSaveData();
	FActorSaveData(const AActor* Actor);

	virtual bool Serialize(FArchive& Acrchive) override;

	TArray<FObjectSaveData> ComponentsSaveData;
	FTransform Transform;
};

/* Container Data */

USTRUCT()
struct FLevelSaveData : public FBaseSaveData
{
	GENERATED_BODY()

public:
	FLevelSaveData();
	FLevelSaveData(const FName& LevelName);

	virtual bool Serialize(FArchive& Archive) override;

	TArray<FActorSaveData> ActorsSaveData;
};

/* SaveGame */

USTRUCT()
struct FGameSaveData : public FBaseSaveData
{
	GENERATED_BODY()

public:
	FGameSaveData();

	virtual bool Serialize(FArchive& Archive) override;

	FName LevelName;
	FLevelSaveData Level;
	FObjectSaveData GameInstance;
	TArray<FLevelSaveData> StreamingLevels;
	FTransform StartTransform;

	bool bIsSerialized;
};