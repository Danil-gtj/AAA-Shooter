// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SaveSubsystemInterface.generated.h"

UINTERFACE(MinimalAPI)
class USaveSubsystemInterface : public UInterface
{
	GENERATED_BODY()
};

class GAMECODE_API ISaveSubsystemInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Save Subsystem")
	void OnLevelDeserialized();
	virtual void OnLevelDeserialized_Implementation() PURE_VIRTUAL(ISaveSubsystemInterface::OnLevelDeserialized_Implementation, );
};
