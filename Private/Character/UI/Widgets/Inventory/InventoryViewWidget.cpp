#include "Character/UI/Widgets/Inventory/InventoryViewWidget.h"
#include "Components/CharacterComponents/CharacterInventoryComponent.h"
#include "Character/UI/Widgets/Inventory/InventorySlotWidget.h"
#include "Components/GridPanel.h"

void UInventoryViewWidget::InitializeViewWidget(TArray<FInventorySlot>& InventorySlots)
{
	for (FInventorySlot& Item : InventorySlots) 
	{
		AddItemInSlotView(Item);
	}
}

void UInventoryViewWidget::AddItemInSlotView(FInventorySlot& SlotToAdd)
{
	checkf(InventorySlotWidgetClass.Get() != nullptr, TEXT("void UInventoryViewWidget::AddItemInSlotView(FInventorySlot& SlotToAdd)"));

	UInventorySlotWidget* SlotWidget = CreateWidget<UInventorySlotWidget>(this, InventorySlotWidgetClass);

	if (SlotWidget != nullptr) 
	{
		SlotWidget->InitializeItemSlot(SlotToAdd);

		const int32 CurrentSlotCount = GridPanelItemSlots->GetChildrenCount();
		const int32 CurrentSlotRow = CurrentSlotCount / ColumnCount;
		const int32 CurrentSlotColumn = CurrentSlotCount % ColumnCount;
		GridPanelItemSlots->AddChildToGrid(SlotWidget, CurrentSlotRow, CurrentSlotColumn);

		SlotWidget->UpdateView();
	}
}
