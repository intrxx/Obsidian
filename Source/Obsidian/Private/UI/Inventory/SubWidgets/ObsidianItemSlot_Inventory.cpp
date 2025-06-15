// Copyright 2024 out of sCope team - Michał Ogiński

#include "UI/Inventory/SubWidgets/ObsidianItemSlot_Inventory.h"

// ~ Core
#include "Components/SizeBox.h"

// ~ Project
#include "ObsidianTypes/ObsidianItemTypes.h"
#include "UI/Inventory/ObsidianInventoryGrid.h"

void UObsidianItemSlot_Inventory::NativeConstruct()
{
	Super::NativeConstruct();

	if(Root_SizeBox)
	{
		Root_SizeBox->SetHeightOverride(ObsidianInventoryItemsStatics::InventorySlotSize.X);
		Root_SizeBox->SetWidthOverride(ObsidianInventoryItemsStatics::InventorySlotSize.Y);
	}
}

void UObsidianItemSlot_Inventory::InitializeSlot(UObsidianInventoryGrid* InOwningGrid, const FIntPoint& InSlotPosition)
{
	OwningGrid = InOwningGrid;
	SlotPosition = InSlotPosition;
}

void UObsidianItemSlot_Inventory::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if(OwningGrid)
	{
		OwningGrid->OnInventorySlotHover(this, true);
	}
}

void UObsidianItemSlot_Inventory::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	if(OwningGrid)
	{
		OwningGrid->OnInventorySlotHover(this, false);
	}
}

FReply UObsidianItemSlot_Inventory::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if(OwningGrid && InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		const bool bShiftDown = InMouseEvent.IsShiftDown();
		OwningGrid->OnInventorySlotMouseButtonDown(this, bShiftDown);
	}
	
	return FReply::Handled();
}




