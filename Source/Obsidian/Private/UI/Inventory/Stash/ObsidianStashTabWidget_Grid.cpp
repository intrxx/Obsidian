// Copyright 2024 out of sCope team - Michał Ogiński

#include "UI/Inventory/Stash/ObsidianStashTabWidget_Grid.h"

#include "UI/Inventory/ObsidianGrid.h"
#include "UI/WidgetControllers/ObsidianInventoryItemsWidgetController.h"

// ~ Core

// ~ Project

void UObsidianStashTabWidget_Grid::InitializeStashTab(UObsidianInventoryItemsWidgetController* InventoryItemsWidgetController, const int32 GridWidth, const int32 GridHeight, const FGameplayTag& InStashTabTag)
{
	if(StashTabGrid && InventoryItemsWidgetController)
	{
		InventoryItemsController = InventoryItemsWidgetController;
		StashTabTag = InStashTabTag;
		StashTabGrid->ConstructGrid(InventoryItemsWidgetController, EObsidianGridOwner::PlayerStash, GridWidth, GridHeight, InStashTabTag);
		StashTabGrid->OnGridSlotPressedDelegate.AddUObject(this, &ThisClass::RequestAddingItemToStashTab);
	}
}

void UObsidianStashTabWidget_Grid::AddItemToStash(UObsidianItem* InItemWidget, const float ItemSlotPadding)
{
	if (StashTabGrid)
	{
		StashTabGrid->AddItemToGrid(InItemWidget, ItemSlotPadding);
	}
}

void UObsidianStashTabWidget_Grid::RequestAddingItemToStashTab(const FIntPoint& ToPosition, const bool bShiftDown) const
{
	if (InventoryItemsController)
	{
		InventoryItemsController->RequestAddingItemToStashTab(FObsidianItemPosition(ToPosition, StashTabTag), bShiftDown);
	}
}
