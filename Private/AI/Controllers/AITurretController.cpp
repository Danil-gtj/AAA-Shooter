// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Controllers/AITurretController.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AIPerceptionComponent.h"
#include "AI/Characters/Turret.h"
#include "Perception/AISense_Damage.h"


void AAITurretController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);
	if (IsValid(InPawn)) 
	{
		checkf(InPawn->IsA<ATurret>(), TEXT("void AAITurretController::SetPawn(APawn* InPawn) can possess only turrets"));
		CachedTurret = StaticCast<ATurret*>(InPawn);
	}
	else
	{
		CachedTurret = nullptr;
	}
}

void AAITurretController::ActorsPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
	if (!CachedTurret.IsValid())
	{
		return;
	}
	 
	AActor* ClosestActor = GetClosestSensedActor(UAISense_Sight::StaticClass());
	AActor* ClosestActorCouser = GetClosestSensedActor(UAISense_Damage::StaticClass());
	if (ClosestActorCouser != nullptr)
	{
		CachedTurret->CurrentTarget = ClosestActorCouser;
	}
	else
	{
		CachedTurret->CurrentTarget = ClosestActor;
	}

	CachedTurret->SetCurrentTarget();
}

	
