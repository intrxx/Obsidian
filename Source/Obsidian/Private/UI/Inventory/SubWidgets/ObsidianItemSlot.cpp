// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/Inventory/SubWidgets/ObsidianItemSlot.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "ObsidianTypes/ObsidianItemTypes.h"

void UObsidianItemSlot::NativeConstruct()
{
	Super::NativeConstruct();

	Action_Image->SetVisibility(ESlateVisibility::Hidden);

	if(Root_SizeBox)
	{
		Root_SizeBox->SetHeightOverride(ObsidianInventoryItemsStatics::InventorySlotSize.X);
		Root_SizeBox->SetWidthOverride(ObsidianInventoryItemsStatics::InventorySlotSize.Y);
	}
}

void UObsidianItemSlot::InitializeSlot(UObsidianInventory* InOwningInventory, const FVector2D& InSlotPosition)
{
	OwningInventory = InOwningInventory;
	SlotPosition = InSlotPosition;
}

void UObsidianItemSlot::InitializeSlot(UObsidianInventory* InOwningInventory, const FGameplayTag& InSlotTag)
{
	OwningInventory = InOwningInventory;
	SlotTag = InSlotTag;
}

void UObsidianItemSlot::SetSlotState(const bool bAvailable)
{
	if(bAvailable)
	{
		Action_Image->SetBrush(SlotGreenLightColor);
		Action_Image->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		Action_Image->SetBrush(SlotRedLightColor);
		Action_Image->SetVisibility(ESlateVisibility::Visible);
	}
}

void UObsidianItemSlot::ResetSlot()
{
	Action_Image->SetVisibility(ESlateVisibility::Hidden);
}

void UObsidianItemSlot::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	OwningInventory->OnHoverOverSlotDelegate.Broadcast(this, true);
}

void UObsidianItemSlot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	OwningInventory->OnHoverOverSlotDelegate.Broadcast(this, false);
}

FReply UObsidianItemSlot::NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if(InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		const bool bShiftDown = InMouseEvent.IsShiftDown();
		OwningInventory->OnMouseButtonDownOnSlotDelegate.Broadcast(this, bShiftDown);
	}
	
	return Super::NativeOnPreviewMouseButtonDown(InGeometry, InMouseEvent);
}

