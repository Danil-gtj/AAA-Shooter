// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Actors/Environment/Interactive/Interface/Interactive.h"
#include "Components/TimelineComponent.h"
#include "Door.generated.h"

UCLASS()
class GAMECODE_API ADoor : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	ADoor();

	virtual void Interact(AGCBaseCharacter* Character) override;

	virtual FName GetActionEventName() const override;

	virtual void Tick(float DeltaTime) override;

	virtual bool HasOnInteractionCallback() const;
	virtual FDelegateHandle AddOnInteractionUFunction(UObject* Object, const FName& FunctionName) override; 
	virtual void RemoveOnInteractionDelegate(FDelegateHandle DeleagetHandle) override;

	//virtual void OnLevelDeserialized_Implementation() override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactive | Door")
	UStaticMeshComponent* DoorMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactive | Door")
	USceneComponent* DoorPivot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactive | Door")
	float AngleClosed = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactive | Door")
	float AngleOpened = 90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interactive | Door")
	UCurveFloat* DoorAnimationCurve;

	virtual void BeginPlay() override;

	IInteractable::FOnInteraction OnInteractionEvent;

private:
	void InteractWithDoor();

	UFUNCTION()
	void UpdateDoorAnimation(float Alpha);

	UFUNCTION()
	void OnDoorAnimationFinished();

	FTimeline DoorOpenAnimTimeLine;

	UPROPERTY()
	bool bIsOpened = false;



};
