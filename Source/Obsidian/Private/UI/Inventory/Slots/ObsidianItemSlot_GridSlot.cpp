// Copyright 2024 out of sCope team - Michał Ogiński

#include "Obsidian/Public/UI/Inventory/Slots/ObsidianItemSlot_GridSlot.h"

#include <Components/SizeBox.h>

#include "ObsidianTypes/ItemTypes/ObsidianItemTypes.h"

void UObsidianItemSlot_GridSlot::NativeConstruct()
{
	Super::NativeConstruct();

	if(Root_SizeBox)
	{
		Root_SizeBox->SetHeightOverride(ObsidianInventoryItemsStatics::InventorySlotSize.X);
		Root_SizeBox->SetWidthOverride(ObsidianInventoryItemsStatics::InventorySlotSize.Y);
	}
}

void UObsidianItemSlot_GridSlot::InitializeSlot(const FIntPoint& InGridSlotPosition)
{
	GridSlotPosition = InGridSlotPosition;
}

FIntPoint UObsidianItemSlot_GridSlot::GetGridSlotPosition() const
{
	return GridSlotPosition;
}

void UObsidianItemSlot_GridSlot::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	OnGridSlotHoverDelegate.Broadcast(this, true);
}

void UObsidianItemSlot_GridSlot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	OnGridSlotHoverDelegate.Broadcast(this, false);
}

FReply UObsidianItemSlot_GridSlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if(InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		FObsidianItemInteractionFlags ItemInteractionFlags;
		ItemInteractionFlags.bItemStacksInteraction = InMouseEvent.IsShiftDown();
		ItemInteractionFlags.bMoveBetweenNextOpenedWindow = InMouseEvent.IsControlDown();
		OnGridSlotLeftButtonPressedDelegate.Broadcast(this, ItemInteractionFlags);
	}
	else if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		FObsidianItemInteractionFlags ItemInteractionFlags;
		ItemInteractionFlags.bItemStacksInteraction = InMouseEvent.IsShiftDown();
		ItemInteractionFlags.bMoveBetweenNextOpenedWindow = InMouseEvent.IsControlDown();
		OnGridSlotRightButtonPressedDelegate.Broadcast(this, ItemInteractionFlags);
	}
	
	return FReply::Handled();
}




