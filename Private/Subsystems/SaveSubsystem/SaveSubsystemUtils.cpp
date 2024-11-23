// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/SaveSubsystem/SaveSubsystemUtils.h"
#include "Subsystems/SaveSubsystem/SaveSubsystemInterface.h"
void USaveSubsystemUtils::BroadcastOnLevelDeserialized(ULevel* Level)
{
	//UE_LOG(LogSaveSubsystem, Display, TEXT(" USaveSubsystemUtils::BroadcastOnLevelDeserialized(): %"), *GameNameSafe(Level));
	if (!IsValid(Level))
	{
		return;
	}

	for (AActor* Actor : Level->Actors)
	{
		if (IsValid(Actor) || Actor->Implements<USaveSubsystemInterface>()) 
		{
			ISaveSubsystemInterface::Execute_OnLevelDeserialized(Actor);
		}
	}
}
