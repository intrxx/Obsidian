// Copyright 2026 out of sCope team - intrxx

#include "Obsidian/Public/UI/InventoryItems/Slots/ObsidianSlot_GridSlot.h"

#include <Components/SizeBox.h>

#include "ObsidianTypes/ItemTypes/ObsidianItemTypes.h"

void UObsidianSlot_GridSlot::NativeConstruct()
{
	Super::NativeConstruct();

	if(Root_SizeBox)
	{
		Root_SizeBox->SetHeightOverride(ObsidianInventoryItemsStatics::InventorySlotSize.X);
		Root_SizeBox->SetWidthOverride(ObsidianInventoryItemsStatics::InventorySlotSize.Y);
	}
}

void UObsidianSlot_GridSlot::InitializeSlot(const FIntPoint& InGridSlotPosition)
{
	GridSlotPosition = InGridSlotPosition;
}

FIntPoint UObsidianSlot_GridSlot::GetGridSlotPosition() const
{
	return GridSlotPosition;
}

void UObsidianSlot_GridSlot::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	OnGridSlotHoverDelegate.Broadcast(this, true);
}

void UObsidianSlot_GridSlot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	OnGridSlotHoverDelegate.Broadcast(this, false);
}

FReply UObsidianSlot_GridSlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
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




