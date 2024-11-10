#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AICharacterSpawner.generated.h"

class IInteractable;
class AGCAICharacter;
UCLASS()
class GAMECODE_API AAICharacterSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	AAICharacterSpawner();

	UFUNCTION()
	void SpawnAI();

	/*virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;*/

protected:

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AISpawner")
	TSubclassOf<AGCAICharacter> CharacterClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AISpawner")
	bool bIsSpawnOnStart;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AISpawner")
	bool bDoOnce = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AISpawner")
	AActor* SpawnTriggerActor;

private:
	bool bCanSpawn = true;

	void UnSubscribeFromTrigger();

	UPROPERTY()
	TScriptInterface<IInteractable> SpawnTrigger;

	FDelegateHandle TriggerHandle;

};
