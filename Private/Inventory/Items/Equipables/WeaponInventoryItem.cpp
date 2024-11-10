// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/Items/Equipables/WeaponInventoryItem.h"

UWeaponInventoryItem::UWeaponInventoryItem()
{
	bIsConsumable = true;
}

void UWeaponInventoryItem::SetEquipWeaponClass(TSubclassOf<AEquipableItem>& WeaponClass)
{
	EquipableWeaponClass = WeaponClass;
}

TSubclassOf<AEquipableItem> UWeaponInventoryItem::GetEquipableWeaponClass() const
{
	return EquipableWeaponClass;
}
