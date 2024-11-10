#include "AI/Spawners/AICharacterSpawner.h"
#include "AI/Characters/GCAICharacter.h"
#include "GameFramework/Actor.h"
#include "Actors/Environment/Interactive/Interface/Interactive.h"
#include "UObject/ScriptInterface.h"

AAICharacterSpawner::AAICharacterSpawner()
{
	USceneComponent* SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SpawnRoot"));
	SetRootComponent(SceneRoot);
}

void AAICharacterSpawner::SpawnAI()
{
	if (!bCanSpawn || !IsValid(CharacterClass)) 
	{
		return;
	}

	AGCAICharacter* AICharacter = GetWorld()->SpawnActor<AGCAICharacter>(CharacterClass, GetTransform());
	if (!IsValid(AICharacter->Controller)) 
	{
		AICharacter->SpawnDefaultController();
	}

	if (bDoOnce) 
	{
		UnSubscribeFromTrigger();
		bCanSpawn = false;
	}
}

//void AAICharacterSpawner::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
//{
//	Super::PostEditChangeProperty(PropertyChangedEvent);
//	if (PropertyChangedEvent.Property->GetName() == GET_MEMBER_NAME_STRING_CHECKED(AAICharacterSpawner, SpawnTriggerActor)) 
//	{
//		 SpawnTrigger = SpawnTriggerActor;
//		 if (SpawnTrigger.GetInterface())
//		 {
//			 if (!SpawnTrigger->HasOnInteractionCallback()) 
//			 {
//				 SpawnTrigger = nullptr;
//				 SpawnTriggerActor = nullptr;
//			 }
//			
//		 }
//		 else
//		 {
//			 SpawnTrigger = nullptr;
//			 SpawnTriggerActor = nullptr;
//		 }
//	}
//}

void AAICharacterSpawner::BeginPlay()
{
	Super::BeginPlay();
	if (SpawnTrigger.GetInterface()) 
	{
		TriggerHandle = SpawnTrigger->AddOnInteractionUFunction(this, FName("SpawnAI"));
	}

	if (bIsSpawnOnStart) 
	{
		SpawnAI();

	}
}

void AAICharacterSpawner::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnSubscribeFromTrigger();
	Super::EndPlay(EndPlayReason);
}

void AAICharacterSpawner::UnSubscribeFromTrigger()
{
	if (TriggerHandle.IsValid() || SpawnTrigger.GetInterface())
	{
		SpawnTrigger->RemoveOnInteractionDelegate(TriggerHandle);
	}
}


