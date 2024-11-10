// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/InteractiveActor.h"
#include "Zipline.generated.h"

UCLASS(Blueprintable)
class GAMECODE_API AZipline : public AInteractiveActor
{
	GENERATED_BODY()
	
public:	
	AZipline();
	 
	virtual void Tick(float DeltaTime) override;

	virtual void OnConstruction(const FTransform& Transform) override;

	FVector StartZiplineVector;
	FVector EndZiplineVector;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zipline | Parametrs")
	UStaticMeshComponent* FirstPillar;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zipline | Parametrs")
	UStaticMeshComponent* SecondPillar;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zipline | Parametrs")
	UStaticMeshComponent* Cable;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Zipline | Parametrs", meta = (MakeEditWidget))
	FVector FirstTrans;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Zipline | Parametrs", meta = (MakeEditWidget))
	FVector SecondTrans;
protected:
	virtual void BeginPlay() override;


};
