// Copyright 2024 out of sCope team - Michał Ogiński

#include "UI/Inventory/Stash/ObsidianStashTabWidget_Slots.h"

#include "Obsidian/ObsidianGameModule.h"
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
	if (StashTab_SlotPanel)
	{
		return StashTab_SlotPanel->GetAllSlots();
	}
	return {};
}

void UObsidianStashTabWidget_Slots::AddItemToStash(UObsidianItem* InItemWidget,
	const FObsidianItemWidgetData& ItemWidgetData)
{
	if(ensure(StashTab_SlotPanel && InItemWidget && ItemWidgetData.ItemPosition.IsOnStashSlot()))
	{
		StashTab_SlotPanel->AddItemWidget(InItemWidget, ItemWidgetData);

		if (ItemWidgetData.bDoesBlockSisterSlot)
		{
			if (const TSubclassOf<UObsidianItem> ItemClass = InItemWidget->GetClass())
			{
				UObsidianItem* BlockingItemWidget = CreateWidget<UObsidianItem>(this, ItemClass);
				BlockingItemWidget->InitializeItemWidget(ItemWidgetData.GridSpan, ItemWidgetData.ItemImage,
					ItemWidgetData.IsItemForSwapSlot());
				StashTab_SlotPanel->AddItemWidget(BlockingItemWidget, ItemWidgetData, true);
			}
			else
			{
				UE_LOG(LogObsidian, Error, TEXT("Unable to construct blockade Item from ItemClass in [%hs]."),
					__FUNCTION__);
			}
		}
	}
}

