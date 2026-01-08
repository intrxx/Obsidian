// Copyright 2024 out of sCope team - Michał Ogiński

#include "UI/Inventory/Stash/ObsidianStashTabWidget_Slots.h"

#include "UI/Inventory/ObsidianSlotPanel.h"
#include "UI/Inventory/Items/ObsidianItem.h"
#include "UI/Inventory/Slots/ObsidianItemSlot_Equipment.h"
#include "UI/WidgetControllers/ObInventoryItemsWidgetController.h"

void UObsidianStashTabWidget_Slots::InitializeStashTab(UObInventoryItemsWidgetController* InInventoryItemsWidgetController,
	const FGameplayTag& InStashTabTag)
{
	if (InInventoryItemsWidgetController && InStashTabTag.IsValid())
	{
		InventoryItemsController = InInventoryItemsWidgetController;
		StashTabTag = InStashTabTag;

		StashTab_SlotPanel->SetWidgetController(InInventoryItemsWidgetController);
		const bool bSuccess = StashTab_SlotPanel->ConstructStashPanel(InStashTabTag);
		ensureMsgf(bSuccess, TEXT("StashTab_SlotPanel was unable to construct the SlotPanel!"));
	}
}

TArray<UObsidianItemSlot_Equipment*> UObsidianStashTabWidget_Slots::GetSlotWidgets() const
{
	return {};
}

void UObsidianStashTabWidget_Slots::AddItemToStash(UObsidianItem* InItemWidget, const float ItemSlotPadding)
{
	if(InItemWidget == nullptr)
	{
		return;
	}

	// const FObsidianItemPosition ItemPosition = InItemWidget->GetItemPosition();
	// if (UObsidianItemSlot_Equipment* EquipmentSlot = FindEquipmentSlotForTag(ItemPosition.GetItemSlotTag()))
	// {
	// 	EquipmentSlot->AddItemToSlot(InItemWidget, ItemSlotPadding);
	// }
}

void UObsidianStashTabWidget_Slots::OnStashSlotHover(UObsidianItemSlot_Equipment* AffectedSlot, const bool bEntered)
{
	if (InventoryItemsController == nullptr || AffectedSlot == nullptr)
	{
		return;
	}

	if (bEntered == false)
	{
		AffectedSlot->SetSlotState(EObsidianItemSlotState::Neutral, EObsidianItemSlotStatePriority::Low);
		return;
	}
	
	if(InventoryItemsController->CanInteractWithPlayerStash() == false)
	{
		AffectedSlot->SetSlotState(EObsidianItemSlotState::RedLight, EObsidianItemSlotStatePriority::Low);
		return;
	}

	if(InventoryItemsController->IsDraggingAnItem() == false)
	{
		AffectedSlot->SetSlotState(EObsidianItemSlotState::Selected, EObsidianItemSlotStatePriority::Low);
		return;
	}
		
	const bool bCanPlace = InventoryItemsController->CanPlaceItemAtStashSlot(FObsidianItemPosition(AffectedSlot->GetSlotTag(), StashTabTag));
	const EObsidianItemSlotState SlotState = bCanPlace ? EObsidianItemSlotState::GreenLight : EObsidianItemSlotState::RedLight;
	AffectedSlot->SetSlotState(SlotState, EObsidianItemSlotStatePriority::Low);
}

void UObsidianStashTabWidget_Slots::OnStashSlotMouseButtonDown(const UObsidianItemSlot_Equipment* AffectedSlot,
	const FObsidianItemInteractionFlags& InteractionFlags) const
{
	if (InventoryItemsController && AffectedSlot)
	{
		InventoryItemsController->RequestAddingItemToStashTab(FObsidianItemPosition(AffectedSlot->GetSlotTag(), StashTabTag),
			InteractionFlags.bItemStacksInteraction);
	}
}
