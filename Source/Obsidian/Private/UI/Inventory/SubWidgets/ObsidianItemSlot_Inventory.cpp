// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/Inventory/SubWidgets/ObsidianItemSlot_Inventory.h"
#include "ObsidianTypes/ObsidianItemTypes.h"
#include "Components/SizeBox.h"

void UObsidianItemSlot_Inventory::NativeConstruct()
{
	Super::NativeConstruct();

	if(Root_SizeBox)
	{
		Root_SizeBox->SetHeightOverride(ObsidianInventoryItemsStatics::InventorySlotSize.X);
		Root_SizeBox->SetWidthOverride(ObsidianInventoryItemsStatics::InventorySlotSize.Y);
	}
}

void UObsidianItemSlot_Inventory::InitializeSlot(UObsidianInventory* InOwningInventory, const FVector2D& InSlotPosition)
{
	OwningInventory = InOwningInventory;
	SlotPosition = InSlotPosition;
}

void UObsidianItemSlot_Inventory::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	OwningInventory->OnHoverOverInventorySlotDelegate.Broadcast(this, true);
}

void UObsidianItemSlot_Inventory::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	OwningInventory->OnHoverOverInventorySlotDelegate.Broadcast(this, false);
}

FReply UObsidianItemSlot_Inventory::NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if(InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		const bool bShiftDown = InMouseEvent.IsShiftDown();
		OwningInventory->OnMouseButtonDownOnInventorySlotDelegate.Broadcast(this, bShiftDown);
	}
	
	return Super::NativeOnPreviewMouseButtonDown(InGeometry, InMouseEvent);
}




