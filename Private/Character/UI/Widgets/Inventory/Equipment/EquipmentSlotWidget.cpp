// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/UI/Widgets/Inventory/Equipment/EquipmentSlotWidget.h"
#include "Inventory/Items/InventoryItem.h"
#include "Utils/GCDataTableUtils.h"
#include "Actors/Equipment/EquipableItem.h"
#include "Inventory/Items/Equipables/WeaponInventoryItem.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Character/UI/Widgets/Inventory/InventorySlotWidget.h"

void UEquipmentSlotWidget::InitializeEquipmentSlot(TWeakObjectPtr<AEquipableItem> Equipment, int32 Index)
{
	if (!Equipment.IsValid())
	{
		return;
	}

	LinkedEquipableItem = Equipment;
	SlotIndexInComponent = Index;

	FWeaponTableRow* EquipmentData = GCDataTableUtils::FindWeaponDate(Equipment->GetDataTableID());
	if (EquipmentData != nullptr)
	{
		LinkedInventoryItem = NewObject<UWeaponInventoryItem>(Equipment->GetOwner());
		LinkedInventoryItem->Initialize(Equipment->GetDataTableID(), EquipmentData->WeaponItemDescription);
		LinkedInventoryItem->SetEquipWeaponClass(EquipmentData->EquipableActor);
	}
}

void UEquipmentSlotWidget::UpdateView()
{
	if (LinkedEquipableItem.IsValid())
	{
		ImageWeaponIcon->SetBrushFromTexture(LinkedInventoryItem->GetDescription().Icon);
		TBWeaponName->SetText(LinkedInventoryItem->GetDescription().Name);
	}
	else
	{
		ImageWeaponIcon->SetBrushFromTexture(nullptr);
		TBWeaponName->SetText(FText::FromName(FName(NAME_None)));
	}
}

FReply UEquipmentSlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (!LinkedEquipableItem.IsValid())
	{
		return FReply::Handled();
	}

	FEventReply Reply = UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton);
	return Reply.NativeReply;
}

void UEquipmentSlotWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	if (!LinkedInventoryItem.IsValid()) 
	{
		return;
	}

	UDragDropOperation* DragOperation = Cast<UDragDropOperation>(UWidgetBlueprintLibrary::CreateDragDropOperation(UDragDropOperation::StaticClass()));

	UInventorySlotWidget* DragWidget = CreateWidget<UInventorySlotWidget>(GetOwningPlayer(), DragAndDropWidgetClass);
	DragWidget->SetItemIcon(LinkedInventoryItem->GetDescription().Icon);

	DragOperation->DefaultDragVisual = DragWidget;
	DragOperation->Pivot = EDragPivot::CenterCenter;
	DragOperation->Payload = LinkedInventoryItem.Get();
	OutOperation = DragOperation;

	LinkedEquipableItem.Reset();
	OnEquipmentRemoveFromSlot.ExecuteIfBound(SlotIndexInComponent);

	UpdateView();
}

bool UEquipmentSlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	const UWeaponInventoryItem* OperationObject = Cast<UWeaponInventoryItem>(InOperation->Payload);
	if (IsValid(OperationObject))
	{
		return OnEquipmentDropInSlot.Execute(OperationObject->GetEquipableWeaponClass(), SlotIndexInComponent);
	}
	return false;
}

void UEquipmentSlotWidget::NativeOnDragCancelled(const FDragDropEvent&, UDragDropOperation* InOperation)
{
	LinkedInventoryItem = Cast<UWeaponInventoryItem>(InOperation->Payload);
	OnEquipmentDropInSlot.Execute(LinkedInventoryItem->GetEquipableWeaponClass(), SlotIndexInComponent);
}
