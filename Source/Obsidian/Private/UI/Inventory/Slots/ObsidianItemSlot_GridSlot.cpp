// Copyright 2024 out of sCope team - Michał Ogiński

#include "Obsidian/Public/UI/Inventory/Slots/ObsidianItemSlot_GridSlot.h"

// ~ Core
#include "Components/SizeBox.h"

// ~ Project
#include "ObsidianTypes/ItemTypes/ObsidianItemTypes.h"
#include "UI/Inventory/ObsidianGrid.h"

void UObsidianItemSlot_GridSlot::NativeConstruct()
{
	Super::NativeConstruct();

	if(Root_SizeBox)
	{
		Root_SizeBox->SetHeightOverride(ObsidianInventoryItemsStatics::InventorySlotSize.X);
		Root_SizeBox->SetWidthOverride(ObsidianInventoryItemsStatics::InventorySlotSize.Y);
	}
}

void UObsidianItemSlot_GridSlot::InitializeSlot(UObsidianGrid* InOwningGrid, const FIntPoint& InSlotGridPosition)
{
	OwningGrid = InOwningGrid;
	GridPosition = InSlotGridPosition;
}

void UObsidianItemSlot_GridSlot::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if(OwningGrid)
	{
		OwningGrid->OnInventorySlotHover(this, true);
	}
}

void UObsidianItemSlot_GridSlot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	if(OwningGrid)
	{
		OwningGrid->OnInventorySlotHover(this, false);
	}
}

FReply UObsidianItemSlot_GridSlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if(OwningGrid && InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		const bool bShiftDown = InMouseEvent.IsShiftDown();
		OwningGrid->OnInventorySlotMouseButtonDown(this, bShiftDown);
	}
	
	return FReply::Handled();
}




