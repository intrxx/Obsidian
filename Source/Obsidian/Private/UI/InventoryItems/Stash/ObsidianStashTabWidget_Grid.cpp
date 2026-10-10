// Copyright 2026 out of sCope team - intrxx

#include "UI/InventoryItems/Stash/ObsidianStashTabWidget_Grid.h"

#include "UI/InventoryItems/ObsidianGridPanel.h"
#include "UI/WidgetControllers/ObInventoryItemsWidgetController.h"


void UObsidianStashTabWidget_Grid::InitializeStashTab(UObInventoryItemsWidgetController* InInventoryItemsWidgetController,
	const int32 InGridWidth, const int32 InGridHeight, const FGameplayTag& InStashTabTag)
{
	if(StashTab_GridPanel && InInventoryItemsWidgetController)
	{
		InventoryItemsController = InInventoryItemsWidgetController;
		StashTabTag = InStashTabTag;
		
		StashTab_GridPanel->SetWidgetController(InInventoryItemsWidgetController);
		const bool bSuccess = StashTab_GridPanel->ConstructStashPanel(InGridWidth, InGridHeight, InStashTabTag);
		ensureMsgf(bSuccess, TEXT("StashTab_GridPanel was unable to construct the GridPanel!"));
	}
}

void UObsidianStashTabWidget_Grid::AddItemToStash(UObsidianItem* InItemWidget,
	const FObsidianItemWidgetData& InItemWidgetData)
{
	if (ensure(StashTab_GridPanel && InItemWidget && InItemWidgetData.ItemPosition.IsOnStashGrid()))
	{
		StashTab_GridPanel->AddItemWidget(InItemWidget, InItemWidgetData);
	}
}

void UObsidianStashTabWidget_Grid::HandleItemChanged(const FObsidianItemWidgetData& InItemWidgetData)
{
	if (ensure(StashTab_GridPanel && InItemWidgetData.ItemPosition.IsOnStashGrid()))
	{
		StashTab_GridPanel->HandleItemChanged(InItemWidgetData);
	}
}

void UObsidianStashTabWidget_Grid::HandleItemRemoved(const FObsidianItemWidgetData& InItemWidgetData)
{
	if (ensure(StashTab_GridPanel && InItemWidgetData.ItemPosition.IsOnStashGrid()))
	{
		StashTab_GridPanel->HandleItemRemoved(InItemWidgetData);
	}
}

void UObsidianStashTabWidget_Grid::HandleHighlightingItems(const TArray<FObsidianItemPosition>& InItemsToHighlight)
{
	if (ensure(StashTab_GridPanel))
	{
		StashTab_GridPanel->HandleHighlightingItems(InItemsToHighlight);
	}
}

void UObsidianStashTabWidget_Grid::ClearUsableItemHighlight()
{
	if (ensure(StashTab_GridPanel))
	{
		StashTab_GridPanel->ClearUsableItemHighlight();
	}
}

