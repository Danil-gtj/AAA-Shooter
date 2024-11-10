// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Environment/Interactive/Pickables/PickablePowerups.h"
#include "GameCode/GameCodeTypes.h"
#include "Inventory/Items/InventoryItem.h"
#include "Utils/GCDataTableUtils.h"
#include "Character/GCBaseCharacter.h"

APickablePowerups::APickablePowerups()
{
	PowerupMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Powerup Mesh"));
	SetRootComponent(PowerupMesh);
}

void APickablePowerups::Interact(AGCBaseCharacter* Character)
{
	/*FItemTableRow* ItemData = GCDataTableUtils::FindInventoryItemData(GetDataTableID());

	if (ItemData == nullptr) 
	{
		return;
	}

	 TWeakObjectPtr<UInventoryItem> Item = TWeakObjectPtr<UInventoryItem>(NewObject<UInventoryItem>(Character, ItemData->InventoryItemClass));
	Item->Initialize(DataTableID, ItemData->InventoryItemDescription);

	const bool bPickedUp = Character->PickupItem(Item);
	if (bPickedUp)
	{
		Destroy();
	}*/
}

FName APickablePowerups::GetActionEventName() const
{
	return ActionInteract;
}
