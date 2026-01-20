// Copyright 2026 out of sCope team - intrxx

#include "UI/Inventory/Stash/ObsidianStashTabWidget_Grid.h"


#include "UI/Inventory/ObsidianGridPanel.h"
#include "UI/WidgetControllers/ObInventoryItemsWidgetController.h"

void UObsidianStashTabWidget_Grid::InitializeStashTab(UObInventoryItemsWidgetController* InventoryItemsWidgetController,
	const int32 GridWidth, const int32 GridHeight, const FGameplayTag& InStashTabTag)
{
	if(StashTab_GridPanel && InventoryItemsWidgetController)
	{
		InventoryItemsController = InventoryItemsWidgetController;
		StashTabTag = InStashTabTag;
		
		StashTab_GridPanel->SetWidgetController(InventoryItemsWidgetController);
		const bool bSuccess = StashTab_GridPanel->ConstructStashPanel(GridWidth, GridHeight, InStashTabTag);
		ensureMsgf(bSuccess, TEXT("StashTab_GridPanel was unable to construct the GridPanel!"));
	}
}

void UObsidianStashTabWidget_Grid::AddItemToStash(UObsidianItem* InItemWidget,
	const FObsidianItemWidgetData& ItemWidgetData)
{
	if (ensure(StashTab_GridPanel && InItemWidget && ItemWidgetData.ItemPosition.IsOnStashGrid()))
	{
		StashTab_GridPanel->AddItemWidget(InItemWidget, ItemWidgetData);
	}
}

void UObsidianStashTabWidget_Grid::HandleItemChanged(const FObsidianItemWidgetData& ItemWidgetData)
{
	if (ensure(StashTab_GridPanel && ItemWidgetData.ItemPosition.IsOnStashGrid()))
	{
		StashTab_GridPanel->HandleItemChanged(ItemWidgetData);
	}
}

void UObsidianStashTabWidget_Grid::HandleItemRemoved(const FObsidianItemWidgetData& ItemWidgetData)
{
	if (ensure(StashTab_GridPanel && ItemWidgetData.ItemPosition.IsOnStashGrid()))
	{
		StashTab_GridPanel->HandleItemRemoved(ItemWidgetData);
	}
}

