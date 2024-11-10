// Fill out your copyright notice in the Description page of Project Settings.


#include "Utils/GCDataTableUtils.h"
#include "Engine/DataTable.h"
#include "Inventory/Items/InventoryItem.h"
#include "GameCode/GameCodeTypes.h"

FWeaponTableRow* GCDataTableUtils::FindWeaponDate(const FName WeaponID)
{
	static const FString contextString(TEXT("Find Weapon Data"));

	UDataTable* WeaponDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/Core/Data/DataTables/DT_WeaponList.DT_WeaponList"));

	if (WeaponDataTable == nullptr)
	{
		return nullptr;
	}

	return WeaponDataTable->FindRow<FWeaponTableRow>(WeaponID, contextString);
}

FItemTableRow* GCDataTableUtils::FindInventoryItemData(const FName ItemID)
{
	static const FString contextString(TEXT("Find Item Data"));

	UDataTable* InventoryItemDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/Core/Data/DataTables/DT_InventoryItemList.DT_InventoryItemList"));

	if (InventoryItemDataTable == nullptr)
	{
		return nullptr;
	}

	return InventoryItemDataTable->FindRow<FItemTableRow>(ItemID, contextString);
}
