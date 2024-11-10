// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AIPatrollingComponent.generated.h"

class APatrollingPath;
UENUM(BlueprintType)
enum class EPatrollingType : uint8
{
	Circle,
	PingPong
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GAMECODE_API UAIPatrollingComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	bool CanPatrol() const;
	FVector GetClosestWayPoint();
	FVector SelectNextWaypoint();

protected:
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Path")
	APatrollingPath* PatrollingPath;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Path")
	EPatrollingType PatrollingType;
	
private:
	bool PingPong = true;

	int32 CurrentWaypointIndex = -1;
};
