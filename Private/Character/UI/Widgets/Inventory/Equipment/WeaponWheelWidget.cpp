// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/UI/Widgets/Inventory/Equipment/WeaponWheelWidget.h"
#include "Components/Image.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/TextBlock.h"
#include "Inventory/Items/InventoryItem.h"
#include "Utils/GCDataTableUtils.h"
#include "Character/CharacterComponents/CharacterEquipmentComponent.h"
#include "GameCode/GameCodeTypes.h"

void UWeaponWheelWidget::InitializeWeaponWheelWidget(UCharacterEquipmentComponent* EquipmentComponent)
{
	LinkedEquipmentComponent = EquipmentComponent;
}

void UWeaponWheelWidget::NextSegment()
{
	/*CurrentSegmentIndex++;
	if (CurrentSegmentIndex == EquipmentSlotsSegments.Num()) 
	{
		CurrentSegmentIndex = 0;
	}
	SelectSegment();*/
}

void UWeaponWheelWidget::PreviousSegment()
{
	/*CurrentSegmentIndex--;
	if (CurrentSegmentIndex < 0)
	{
		CurrentSegmentIndex = EquipmentSlotsSegments.Num() < 1;
	}
	SelectSegment();*/
}

void UWeaponWheelWidget::ConfirmSelection()
{
	/*EEquipmentSlots SellectedSlot = EquipmentSlotsSegments[CurrentSegmentIndex];
	LinkedEquipmentComponent->EquipItemInSlot(SellectedSlot);
	RemoveFromParent();*/
}

void UWeaponWheelWidget::NativeConstruct()
{
	Super::NativeConstruct();
	/*if (IsValid(RadialBackground) && !IsValid(BackgroundMaterial))
	{
		BackgroundMaterial = RadialBackground->GetDynamicMaterial();
		BackgroundMaterial->SetScalarParameterValue(FName("Segments"), EquipmentSlotsSegments.Num());
	}*/
}

void UWeaponWheelWidget::SelectSegment()
{
	//BackgroundMaterial->SetScalarParameterValue(FName("Index"), CurrentSegmentIndex);
	/*FWeaponTableRow* WeaponData = GetTableRowForSegment(CurrentSegmentIndex);
	if (WeaponData == nullptr)
	{
		WeaponNameText->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		WeaponNameText->SetVisibility(ESlateVisibility::Visible);
		WeaponNameText->SetText(WeaponData->WeaponItemDescription.Name);
	}*/
}

FWeaponTableRow* UWeaponWheelWidget::GetTableRowForSegment(int32 SegmentIndex) const
{
	/*const EEquipmentSlots& SegmentSlot = EquipmentSlotsSegments[SegmentIndex];
	AEquipableItem* EquipableItem = LinkedEquipmentComponent->GetItems()[(int32)SegmentSlot];
	if (!IsValid(EquipableItem))
	{
		return nullptr;
	}
	return GCDataTableUtils::FindWeaponDate(EquipableItem->GetDataTableID());*/
	return nullptr;
}


