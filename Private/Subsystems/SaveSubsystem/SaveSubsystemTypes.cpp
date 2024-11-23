#include "Subsystems/SaveSubsystem/SaveSubsystemTypes.h"
#include "Subsystems/SaveSubsystem/SaveSubsystem.h"
#include "Subsystems/Streaming/StreamingSubsystemManager.h"

DEFINE_LOG_CATEGORY(LogSaveSubsystem);

FSaveSubsystemArchive::FSaveSubsystemArchive(FArchive& InInnerArchive, bool bInLoadIfFindFails)
	: FObjectAndNameAsStringProxyArchive(InInnerArchive, bInLoadIfFindFails)
{
	ArIsSaveGame = true;
	ArNoDelta = true;
}

FSaveDirectoryVisitor::FSaveDirectoryVisitor(TArray<int32>& InSavesIds)
	: SaveIds(InSavesIds)
{
}

bool FSaveDirectoryVisitor::Visit(const TCHAR* FilenameOrDirectory, bool bIsDirectory)
{
	if (bIsDirectory)
	{
		return true;
	}

	const FString FullFilePath(FilenameOrDirectory);

	FString DirectoryName;
	FString FileName;
	FString FileExtension; 
	FPaths::Split(FullFilePath, DirectoryName, FileName, FileExtension);
	if (FileExtension == FileExtensionSave.ToString())
	{
		const int32 SaveId = FCString::Atoi(*FileName);
		if (SaveId > 0)
		{
			SaveIds.AddUnique(SaveId);
		}
	}

	return true;
}

BoolScopeWrapper::BoolScopeWrapper(bool& bInValue, bool bNewValue)
	: bValue(bInValue)
	, bInitialValue(bInValue)
{
	bValue = bNewValue;
}

BoolScopeWrapper::~BoolScopeWrapper()
{
	bValue = bInitialValue;
}

UStreamingLevelObserver::UStreamingLevelObserver()
{
}

void UStreamingLevelObserver::Initialize(USaveSubsystem* InSaveSubsystem, ULevelStreaming* InStreamingLevel)
{
	UE_LOG(LogSaveSubsystem, Display, TEXT("UStreamingLevelObserver::Initialize(): StreamingLevel %s"), *GetNameSafe(InStreamingLevel));
	if (!IsValid(InSaveSubsystem) || !IsValid(InStreamingLevel))
	{
		return;
	}

	SaveSubsystem = InSaveSubsystem;
	StreamingLevel = InStreamingLevel;

	StreamingLevel->OnLevelShown.AddUniqueDynamic(this, &UStreamingLevelObserver::OnLevelShown);
	StreamingLevel->OnLevelHidden.AddUniqueDynamic(this, &UStreamingLevelObserver::OnLevelHidden);
}

void UStreamingLevelObserver::Deinitialize()
{
	UE_LOG(LogSaveSubsystem, Display, TEXT("UStreamingLevelObserver::Deinitialize(): StreamingLevel %s"), *GetNameSafe(StreamingLevel.Get()));

	if (StreamingLevel.IsValid())
	{
		StreamingLevel->OnLevelShown.RemoveDynamic(this, &UStreamingLevelObserver::OnLevelShown);
		StreamingLevel->OnLevelHidden.RemoveDynamic(this, &UStreamingLevelObserver::OnLevelHidden);
	}

	StreamingLevel.Reset();
	SaveSubsystem.Reset();
}

void UStreamingLevelObserver::Serialize(FArchive& Archive)
{
	LevelSaveData.Serialize(Archive);
}

void UStreamingLevelObserver::OnLevelShown()
{
	UE_LOG(LogSaveSubsystem, Display, TEXT("UStreamingLevelObserver::OnLevelShown(): StreamingLevel %s"), *GetNameSafe(StreamingLevel.Get()));
	if (!SaveSubsystem.IsValid() || !StreamingLevel.IsValid())
	{
		return;
	}

	SaveSubsystem->DeserializeLevel(StreamingLevel->GetLoadedLevel(), StreamingLevel.Get());
}

void UStreamingLevelObserver::OnLevelHidden()
{
	UE_LOG(LogSaveSubsystem, Display, TEXT("UStreamingLevelObserver::OnLevelHidden(): StreamingLevel %s"), *GetNameSafe(StreamingLevel.Get()));
	if (!SaveSubsystem.IsValid() || !StreamingLevel.IsValid())
	{
		return;
	}

	SaveSubsystem->SerializeLevel(StreamingLevel->GetLoadedLevel(), StreamingLevel.Get());
}

