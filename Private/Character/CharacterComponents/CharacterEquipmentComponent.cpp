#include "Character/CharacterComponents/CharacterEquipmentComponent.h"
#include "Character/GCBaseCharacter.h"
#include "Actors/Equipment/Weapons/RangeWeaponItem.h"
#include "GameCode/GameCodeTypes.h"
#include "Actors/Equipment/Throwables/ThrowableItem.h"
#include "Net/UnrealNetwork.h"
#include "Character/UI/Widgets/Inventory/Equipment/EquipmentViewWidget.h"
#include "Character/UI/Widgets/Inventory/Equipment/WeaponWheelWidget.h"

void UCharacterEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();
	checkf(GetOwner()->IsA<AGCBaseCharacter>(), TEXT(" UCharacterEquipmentComponent::BeginPlay() CharacterEquipmentComponent can be used only with GCBaseCharacter."));
	CachedBaseCharacter = StaticCast<AGCBaseCharacter*>(GetOwner());
	if (CachedBaseCharacter.IsValid())
	{
		CreateLoadout();
		AutoEquip();
	}
}


UCharacterEquipmentComponent::UCharacterEquipmentComponent()
{
	SetIsReplicatedByDefault(true);
}

void UCharacterEquipmentComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UCharacterEquipmentComponent, CurrentEquippedSlot);
	DOREPLIFETIME(UCharacterEquipmentComponent, AmunitionArray);
	DOREPLIFETIME(UCharacterEquipmentComponent, ItemsArray);
}

EEquipableItemType UCharacterEquipmentComponent::GetCurrentEquippedItemType() const
{
	EEquipableItemType Result = EEquipableItemType::None;
	if (IsValid(CurrentEquippedItem)) 
	{
		Result = CurrentEquippedItem->GetItemType();
	}
	return Result;
}

ARangeWeaponItem* UCharacterEquipmentComponent::GetCurrentRrangeWeapon() const
{
	return CurrentEquippedWeapon;
}

AMeleeWeaponItem* UCharacterEquipmentComponent::GetCurrentMeleeWeaponItem() const
{
	return CurrentMeleeWeapon;
}

bool UCharacterEquipmentComponent::IsEquipping() const
{
	return bIsEquipping;
}
 
void UCharacterEquipmentComponent::ReloadCurrenWeapon()
{
	check(IsValid(CurrentEquippedWeapon));
	int32 AvailableAmunition = GetAvalibleAmunitionForWeapon();
	if (AvailableAmunition <= 0) 
	{
		return;
	}

	if (GetOwner()->GetLocalRole() == ROLE_AutonomousProxy)
	{
		CurrentEquippedWeapon->Server_Reliable_StartReaload();
	}

	CurrentEquippedWeapon->StartReload();
}


void UCharacterEquipmentComponent::EquipItemInSlot(EEquipmentSlots Slot)
{
	if (bIsEquipping)
	{
		return;
	}

	UnEquipCurrentItem();
	CurrentEquippedItem = ItemsArray[(int32)Slot];
	CurrentEquippedWeapon = Cast<ARangeWeaponItem>(CurrentEquippedItem);
	CurrentThrowableItem = Cast<AThrowableItem>(CurrentEquippedItem);
	CurrentMeleeWeapon = Cast<AMeleeWeaponItem>(CurrentEquippedItem);

	if (CurrentEquippedItem == CurrentThrowableItem && FragGranades <= 0)
	{
		return;
	}

	if (IsValid(CurrentEquippedItem)) 
	{
		UAnimMontage* EquipMontage = CurrentEquippedItem->GetChracterEquipAnimMontage();
		if (IsValid(EquipMontage)) 
		{
			bIsEquipping = true;
			float EquipDuration = CachedBaseCharacter->PlayAnimMontage(EquipMontage);
			GetWorld()->GetTimerManager().SetTimer(EquipTimer, this, &UCharacterEquipmentComponent::EquipAnimationFinished, EquipDuration, false);
		}
		else
		{
			AttachCurrentItemToEquippedSocket();
		}
		CurrentEquippedItem->Equip();
	} 

	if (IsValid(CurrentEquippedWeapon))
	{
		OnCurrentWeaponAmmoChangedHandle = CurrentEquippedWeapon->OnAmmoChanged.AddUFunction(this, FName("OnCurrentWeaponAmmoChanged"));
		OnCurrentWeaponReloadHandle = CurrentEquippedWeapon->OnReloadComplete.AddUFunction(this, FName("OnWeaponReloadComplite"));
		OnCurrentWeaponAmmoChanged(CurrentEquippedWeapon->GetAmmo());
	}

	if (OnEquippedItemChanged.IsBound())
	{
		OnEquippedItemChanged.Broadcast(CurrentEquippedItem);
	}

	CurrentEquippedSlot = Slot;
	if (GetOwner()->GetLocalRole() == ROLE_AutonomousProxy) 
	{
		Server_EquipItemInSlot(CurrentEquippedSlot);
	}
}

void UCharacterEquipmentComponent::AttachCurrentItemToEquippedSocket()
{
	if (IsValid(CurrentEquippedItem)) 
	{
		CurrentEquippedItem->AttachToComponent(CachedBaseCharacter->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, CurrentEquippedItem->GetEquippedSocketName());
	}
}

void UCharacterEquipmentComponent::LaunchCurrentThrowableItem()
{
	if (CurrentThrowableItem)
	{
		if (GetOwner()->GetLocalRole() == ROLE_Authority)
		{
			CurrentThrowableItem->Throw();
		}
		else
		{
			Server_LaunchCurrentThrowableItem();
		}

		bIsEquipping = false;
		EquipItemInSlot(PreviusEquippedSlot);
		FragGranades--;
		OnCurrentFragGranadesChanged(FragGranades);
	}
}

void UCharacterEquipmentComponent::UnEquipCurrentItem()
{
	if (IsValid(CurrentEquippedItem))
	{
		CurrentEquippedItem->AttachToComponent(CachedBaseCharacter->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, CurrentEquippedItem->GetUnEquippedSocketName());
		CurrentEquippedItem->UnEquip();
	}

	if (IsValid(CurrentEquippedWeapon))
	{
		CurrentEquippedWeapon->StopFire();
		CurrentEquippedWeapon->EndReload(false);
		CurrentEquippedWeapon->OnAmmoChanged.Remove(OnCurrentWeaponAmmoChangedHandle);
		CurrentEquippedWeapon->OnReloadComplete.Remove(OnCurrentWeaponReloadHandle);
	}
	
	PreviusEquippedSlot = CurrentEquippedSlot;
	CurrentEquippedSlot = EEquipmentSlots::None;
}

void UCharacterEquipmentComponent::EquipNextItem()
{
	//if (CachedBaseCharacter->IsLocallyControlled()) 
	//{
	//	if (IsSelectingWeapon()) 
	//	{
	//		WeaponWheelWidget->NextSegment();
	//	}
	//	else
	//	{
	//		APlayerController* PlayerController = CachedBaseCharacter->GetController<APlayerController>();
	//		OpneWeaponWheel(PlayerController);
	//	}
	//	return;
	//}

	uint32 CurrentSlotIndex = (uint32)CurrentEquippedSlot;
	uint32 NextSlotIndex = NextItemsArraySlotIndex(CurrentSlotIndex);
	while (CurrentSlotIndex != NextSlotIndex && !IsValid(ItemsArray[NextSlotIndex]) && IgnoreSlotWhileSwitching.Contains((EEquipmentSlots)NextSlotIndex))
	{
		NextSlotIndex = NextItemsArraySlotIndex(NextSlotIndex);
	}
	if (CurrentSlotIndex != NextSlotIndex) 
	{
		EquipItemInSlot((EEquipmentSlots)NextSlotIndex);
	}
}

void UCharacterEquipmentComponent::EquipPreviousItem()
{
	/*if (CachedBaseCharacter->IsLocallyControlled())
	{
		if (IsSelectingWeapon())
		{
			WeaponWheelWidget->PreviousSegment();
		}
		else
		{
			APlayerController* PlayerController = CachedBaseCharacter->GetController<APlayerController>();
			OpneWeaponWheel(PlayerController);
		}
		return;
	}*/

	uint32 CurrentSlotIndex = (uint32)CurrentEquippedSlot;
	uint32 PreviousSlotIndex = PreviousItemsSlotIndex(CurrentSlotIndex);

	while (CurrentSlotIndex != PreviousSlotIndex && !IsValid(ItemsArray[PreviousSlotIndex]) && IgnoreSlotWhileSwitching.Contains((EEquipmentSlots)PreviusEquippedSlot))
	{
		PreviousSlotIndex = PreviousItemsSlotIndex(PreviousSlotIndex);
	}
	if (CurrentSlotIndex != PreviousSlotIndex)
	{
		EquipItemInSlot((EEquipmentSlots)PreviousSlotIndex);
	}
}

bool UCharacterEquipmentComponent::AddEquipmentItem(const TSubclassOf<AEquipableItem> EquipableItemClass, int32 SlotIndex)
{
	if (!IsValid(EquipableItemClass)) 
	{
		return false;
	}

	AEquipableItem* DefaultItemObject = EquipableItemClass->GetDefaultObject<AEquipableItem>();
	if (!DefaultItemObject->IsSlotCompatible((EEquipmentSlots)SlotIndex))
	{
		return false;
	}

	if (!IsValid(ItemsArray[SlotIndex]))
	{
		AEquipableItem* Item = GetWorld()->SpawnActor<AEquipableItem>(EquipableItemClass);
		Item->AttachToComponent(CachedBaseCharacter->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, Item->GetUnEquippedSocketName());
		Item->SetOwner(CachedBaseCharacter.Get());
		Item->UnEquip();
		ItemsArray[SlotIndex] = Item;
	}
	else if(DefaultItemObject->IsA<ARangeWeaponItem>())
	{
		ARangeWeaponItem* RangeWeaponObject = StaticCast<ARangeWeaponItem*>(DefaultItemObject);
		int32 AmmoSlotIndex = (int32)RangeWeaponObject->GetAmmoType();
		AmunitionArray[SlotIndex] += RangeWeaponObject->GetMaxAmmo();

		AmunitionArray[(uint32)RangeWeaponObject->GetAmmoType()] += RangeWeaponObject->GetMaxAmmo();
		if (IsValid(CurrentEquippedWeapon))
		{
			OnCurrentWeaponAmmoChanged(CurrentEquippedWeapon->GetAmmo());
		}
	}

	return true;
}

void UCharacterEquipmentComponent::RemoveItemFromSlot(int32 SlotIndex)
{
	if((uint32)CurrentEquippedSlot == SlotIndex)
	{
		UnEquipCurrentItem();
	}
	ItemsArray[SlotIndex]->Destroy();
	ItemsArray[SlotIndex] = nullptr;
}

void UCharacterEquipmentComponent::CloseViewEquipment()
{
	if (ViewWidget->IsVisible())
	{
		ViewWidget->RemoveFromParent();
	}
}

bool UCharacterEquipmentComponent::IsViewVisible() const
{
	bool Result = false;
	if (IsValid(ViewWidget)) 
	{
		Result = ViewWidget->IsVisible();
	}
	return Result;
}

bool UCharacterEquipmentComponent::IsSelectingWeapon() const
{
	return IsValid(WeaponWheelWidget) && WeaponWheelWidget->IsVisible();
}

void UCharacterEquipmentComponent::ConfirmWeaponSelection()
{
	WeaponWheelWidget->ConfirmSelection();
}

const TArray<AEquipableItem*>& UCharacterEquipmentComponent::GetItems() const
{
	return ItemsArray;
}

void UCharacterEquipmentComponent::OnLevelDeserialized_Implementation()
{
	EquipItemInSlot(CurrentEquippedSlot);
}

void UCharacterEquipmentComponent::Server_EquipItemInSlot_Implementation(EEquipmentSlots Slot)
{
	EquipItemInSlot(Slot);
}

void UCharacterEquipmentComponent::Server_LaunchCurrentThrowableItem_Implementation()
{
		CurrentThrowableItem->Throw();
}

void UCharacterEquipmentComponent::OnRep_ItemsArray()
{
	for (AEquipableItem* Item : ItemsArray)
	{
		if (IsValid(Item))
		{
			Item->Equip();
		}
	}
}

void UCharacterEquipmentComponent::CreateLoadout()
{
	if (GetOwner()->GetLocalRole() < ROLE_Authority)
	{
		return;
	}

	AmunitionArray.AddZeroed((uint32)EAmunitionType::MAX);
	for (const TPair<EAmunitionType, int32>& AmmoPair : MaxAmunitionAmount)
	{
		AmunitionArray[(uint32)AmmoPair.Key] = FMath::Max(AmmoPair.Value, 0);
	}

	ItemsArray.AddZeroed((uint32)EEquipmentSlots::MAX);
	for (const TPair<EEquipmentSlots, TSubclassOf<AEquipableItem>> & ItemPair : ItemsLoadout)
	{
		if (!IsValid(ItemPair.Value))
		{
			continue;
		}

		AddEquipmentItem(ItemPair.Value, (int32)ItemPair.Key);
	}
}

void UCharacterEquipmentComponent::AutoEquip()
{
	if (AutoEquipItemInSlot != EEquipmentSlots::None)
	{
		EquipItemInSlot(AutoEquipItemInSlot);
	}
}



void UCharacterEquipmentComponent::EquipAnimationFinished()
{
	bIsEquipping = false;
	AttachCurrentItemToEquippedSocket();
}

int32 UCharacterEquipmentComponent::GetAvalibleAmunitionForWeapon()
{
	check(GetCurrentRrangeWeapon());
	return AmunitionArray[(uint32)GetCurrentRrangeWeapon()->GetAmmoType()];
}

uint32 UCharacterEquipmentComponent::NextItemsArraySlotIndex(uint32 CurrentSlotIndex)
{
	if (CurrentSlotIndex == ItemsArray.Num() - 1) {
		return 0;
	}
	else
	{
		return CurrentSlotIndex + 1;
	}
}

uint32 UCharacterEquipmentComponent::PreviousItemsSlotIndex(uint32 CurrentSlotIndex)
{
	if (CurrentSlotIndex == 0) {
		return ItemsArray.Num() - 1;
	}
	else
	{
		return CurrentSlotIndex - 1;
	}
}

void UCharacterEquipmentComponent::OnWeaponReloadComplite()
{
	ReloadAmmoInCurrentWeapon();
}

void UCharacterEquipmentComponent::ReloadAmmoInCurrentWeapon(int32 NumberOfAmmo, bool bCheckIsFull)
{
	int32 AvailableAmunition = GetAvalibleAmunitionForWeapon();
	int32 CurrenAmmo = CurrentEquippedWeapon->GetAmmo();
	int32 AmmoToReload = CurrentEquippedWeapon->GetMaxAmmo() - CurrenAmmo;
	int32 ReloadedAmmo = FMath::Min(/*FMath::Min(*/AvailableAmunition, AmmoToReload/*), NumberOfAmmo*/);
	if (NumberOfAmmo > 0)
	{
		ReloadedAmmo = FMath::Min(ReloadedAmmo, NumberOfAmmo);
	}
	AmunitionArray[(uint32)CurrentEquippedWeapon->GetAmmoType()] -= ReloadedAmmo;
	CurrentEquippedWeapon->SetAmmo(ReloadedAmmo + CurrenAmmo);

	if (bCheckIsFull)
	{
		AvailableAmunition = AmunitionArray[(uint32)CurrentEquippedWeapon->GetAmmoType()];
		bool bIsFullyReloadded = CurrentEquippedWeapon->GetAmmo() == CurrentEquippedWeapon->GetMaxAmmo();
		if (AvailableAmunition == 0 || bIsFullyReloadded)
		{
			CurrentEquippedWeapon->EndReload(true);
		}
	}
}

void UCharacterEquipmentComponent::OnCurrentWeaponAmmoChanged(int32 Ammo)
{
	if (OnCurrentWeaponAmmoChangedEvent.IsBound())
	{
		OnCurrentWeaponAmmoChangedEvent.Broadcast(Ammo, GetAvalibleAmunitionForWeapon());
	}
}

void UCharacterEquipmentComponent::OnCurrentFragGranadesChanged(int32 FragGranades_In)
{
	if (OnCurrentFragGranadesChangedEvent.IsBound())
	{
		OnCurrentFragGranadesChangedEvent.Broadcast(FragGranades_In);
	}
}

void UCharacterEquipmentComponent::OnRep_CurrentEquipSlot(EEquipmentSlots CurrentEquipSlotOld)
{
	EquipItemInSlot(CurrentEquippedSlot);
}

