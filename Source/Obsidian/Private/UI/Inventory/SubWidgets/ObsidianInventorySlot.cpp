// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/Inventory/SubWidgets/ObsidianInventorySlot.h"

#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "ObsidianTypes/ObsidianItemTypes.h"

void UObsidianInventorySlot::NativeConstruct()
{
	Super::NativeConstruct();

	Action_Image->SetVisibility(ESlateVisibility::Hidden);

	if(Root_SizeBox)
	{
		Root_SizeBox->SetHeightOverride(ObsidianInventoryItemsStatics::InventorySlotSize.X);
		Root_SizeBox->SetWidthOverride(ObsidianInventoryItemsStatics::InventorySlotSize.Y);
	}
}

void UObsidianInventorySlot::SetSlotState(const bool bAvailable)
{
	if(bAvailable)
	{
		Action_Image->SetBrush(SlotAvailableColor);
		Action_Image->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		Action_Image->SetBrush(SlotTakenColor);
		Action_Image->SetVisibility(ESlateVisibility::Visible);
	}
}

void UObsidianInventorySlot::ResetSlot()
{
	Action_Image->SetVisibility(ESlateVisibility::Hidden);
}

void UObsidianInventorySlot::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	//Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	OnHoverOverSlotDelegate.Broadcast(true, this);
}

void UObsidianInventorySlot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	//Super::NativeOnMouseLeave(InMouseEvent);
	OnHoverOverSlotDelegate.Broadcast(false, this);
}

FReply UObsidianInventorySlot::NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if(InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		const bool bShiftDown = InMouseEvent.IsShiftDown();
		OnMouseButtonDownOnSlotDelegate.Broadcast(SlotPosition, bShiftDown);
	}
	
	return Super::NativeOnPreviewMouseButtonDown(InGeometry, InMouseEvent);
}

