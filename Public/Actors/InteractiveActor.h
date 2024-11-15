// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractiveActor.generated.h"

UCLASS(Abstract)
class GAMECODE_API AInteractiveActor : public AActor
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

	bool IsOverlappingCharacterCapsule(AActor* OtherActor, UPrimitiveComponent* OtherComp);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	class UPrimitiveComponent* InteractionVolume;

	UFUNCTION()
	virtual void OnInteractionVolumeOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	virtual void OnInteractionVolumeOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
