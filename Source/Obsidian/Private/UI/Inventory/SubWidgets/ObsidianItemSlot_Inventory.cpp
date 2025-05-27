// Copyright 2024 out of sCope team - Michał Ogiński

#include "UI/Inventory/SubWidgets/ObsidianItemSlot_Inventory.h"

// ~ Core
#include "Components/SizeBox.h"

// ~ Project
#include "ObsidianTypes/ObsidianItemTypes.h"

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
	if(OwningInventory)
	{
		OwningInventory->OnInventorySlotHover(this, true);
	}
}

void UObsidianItemSlot_Inventory::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	if(OwningInventory)
	{
		OwningInventory->OnInventorySlotHover(this, false);
	}
}

FReply UObsidianItemSlot_Inventory::NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if(OwningInventory && InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		const bool bShiftDown = InMouseEvent.IsShiftDown();
		OwningInventory->OnInventorySlotMouseButtonDown(this, bShiftDown);
	}
	
	return Super::NativeOnPreviewMouseButtonDown(InGeometry, InMouseEvent);
}




