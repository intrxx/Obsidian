// Copyright 2026 out of sCope team - intrxx

#include "UI/InventoryItems/Stash/ObsidianStashTabWidget_Slots.h"

#include "Obsidian/ObsidianLogCategories.h"
#include "UI/InventoryItems/Items/ObsidianItem.h"
#include "UI/InventoryItems/ObsidianSlotPanel.h"
#include "UI/InventoryItems/Slots/ObsidianSlot_ItemSlot.h"
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

TArray<UObsidianSlot_ItemSlot*> UObsidianStashTabWidget_Slots::GetSlotWidgets() const
{
	if (StashTab_SlotPanel)
	{
		return StashTab_SlotPanel->GetAllSlots();
	}
	return {};
}

void UObsidianStashTabWidget_Slots::AddItemToStash(UObsidianItem* InItemWidget,
	const FObsidianItemWidgetData& InItemWidgetData)
{
	if(ensure(StashTab_SlotPanel && InItemWidget && InItemWidgetData.ItemPosition.IsOnStashSlot()))
	{
		StashTab_SlotPanel->AddItemWidget(InItemWidget, InItemWidgetData);

		if (InItemWidgetData.bDoesBlockSisterSlot)
		{
			if (const TSubclassOf<UObsidianItem> ItemClass = InItemWidget->GetClass())
			{
				UObsidianItem* BlockingItemWidget = CreateWidget<UObsidianItem>(this, ItemClass);
				BlockingItemWidget->InitializeItemWidget(InItemWidgetData.GridSpan, InItemWidgetData.ItemImage,
					InItemWidgetData.IsItemForSwapSlot());
				StashTab_SlotPanel->AddItemWidget(BlockingItemWidget, InItemWidgetData, true);
			}
			else
			{
				UE_LOG(ObLogItems, Error, TEXT("Unable to construct blockade Item from ItemClass in [%hs]."),
					__FUNCTION__);
			}
		}
	}
}

void UObsidianStashTabWidget_Slots::HandleItemChanged(const FObsidianItemWidgetData& InItemWidgetData)
{
	if (ensure(StashTab_SlotPanel && InItemWidgetData.ItemPosition.IsOnStashSlot()))
	{
		StashTab_SlotPanel->HandleItemChanged(InItemWidgetData);
	}
}

void UObsidianStashTabWidget_Slots::HandleItemRemoved(const FObsidianItemWidgetData& InItemWidgetData)
{
	if (ensure(StashTab_SlotPanel && InItemWidgetData.ItemPosition.IsOnStashSlot()))
	{
		StashTab_SlotPanel->HandleItemRemoved(InItemWidgetData);
	}
}

void UObsidianStashTabWidget_Slots::HandleHighlightingItems(const TArray<FObsidianItemPosition>& InItemsToHighlight)
{
	if (ensure(StashTab_SlotPanel))
	{
		StashTab_SlotPanel->HandleHighlightingItems(InItemsToHighlight);
	}
}

void UObsidianStashTabWidget_Slots::ClearUsableItemHighlight()
{
	if (ensure(StashTab_SlotPanel))
	{
		StashTab_SlotPanel->ClearUsableItemHighlight();
	}
}



