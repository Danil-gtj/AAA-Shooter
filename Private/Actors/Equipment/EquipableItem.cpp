#include "Actors/Equipment/EquipableItem.h"

AEquipableItem::AEquipableItem()
{
	SetReplicates(true);
}

EEquipableItemType AEquipableItem::GetItemType() const
{
	return ItemType;
}

UAnimMontage* AEquipableItem::GetChracterEquipAnimMontage() const
{
	return ChracterEquipAnimMontage;
}

FName AEquipableItem::GetUnEquippedSocketName() const
{
	return UnEquippedSocketName;
}

FName AEquipableItem::GetEquippedSocketName() const
{
	return EquippedSocketName;
}

void AEquipableItem::Equip()
{
	if (OnEquipmentStateChanged.IsBound())
	{
		OnEquipmentStateChanged.Broadcast(true);
	}
}

void AEquipableItem::UnEquip()
{
	if (OnEquipmentStateChanged.IsBound())
	{
		OnEquipmentStateChanged.Broadcast(false);
	}
}

EReticleType AEquipableItem::GetReticleType() const
{
	return ReticleType;
}

void AEquipableItem::SetOwner(AActor* NewOwner)
{
	Super::SetOwner(NewOwner);
	if (IsValid(NewOwner))
	{
		checkf(GetOwner()->IsA<AGCBaseCharacter>(), TEXT("void AEquipableItem::SetOwner("));
		CacheBaseCharacter = StaticCast<AGCBaseCharacter*>(GetOwner());
		if (GetLocalRole() == ROLE_Authority) 
		{
			SetAutonomousProxy(true);
		}
	}
	else
	{
		CacheBaseCharacter = nullptr;
	}
}
FName AEquipableItem::GetDataTableID() const
{
	return DataTableID;
}

bool AEquipableItem::IsSlotCompatible(EEquipmentSlots Slot)
{
	return CompatableEquipmentSlots.Contains(Slot);
}


AGCBaseCharacter* AEquipableItem::GetCharacterOwner() const
{
	return CacheBaseCharacter.IsValid() ? CacheBaseCharacter.Get() : nullptr;
}
