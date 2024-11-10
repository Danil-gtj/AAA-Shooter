// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTServices/BTService_Fire.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/GCBaseCharacter.h"
#include "Actors/Equipment/Weapons/RangeWeaponItem.h"

UBTService_Fire::UBTService_Fire()
{
	NodeName = "Fire";
}

void UBTService_Fire::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	AAIController* AIController = OwnerComp.GetAIOwner();
	UBlackboardComponent* BlackBoard = OwnerComp.GetBlackboardComponent();

	if (!IsValid(AIController) || !IsValid(BlackBoard))
	{
		return;
	}

	AGCBaseCharacter* Character = Cast<AGCBaseCharacter>(AIController->GetPawn());
	if (!IsValid(Character))
	{
		return;
	}

	const UCharacterEquipmentComponent* EquipmentComponent = Character->GetCharacterEquipmentComponent();
	ARangeWeaponItem* RangeWeapon = EquipmentComponent->GetCurrentRrangeWeapon();

	if (!IsValid(RangeWeapon)) 
	{
		return;
	}

	AActor* CurrentTarget =Cast<AActor>(BlackBoard->GetValueAsObject(TargetKey.SelectedKeyName));
	if (!IsValid(CurrentTarget)) 
	{
		Character->StopFire();
		return;
	}

	float DistSq = FVector::DistSquared(CurrentTarget->GetActorLocation(), Character->GetActorLocation());
	if (DistSq > FMath::Square(MaxFireDistance)) 
	{
		Character->StopFire();
		return;
	}

	if (!(RangeWeapon->IsReloading() || RangeWeapon->IsFiring()))
	{
		Character->StartFire();
	}
}
