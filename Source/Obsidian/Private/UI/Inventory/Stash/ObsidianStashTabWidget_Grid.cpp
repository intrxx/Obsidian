// Copyright 2024 out of sCope team - Michał Ogiński

#include "UI/Inventory/Stash/ObsidianStashTabWidget_Grid.h"

#include "UI/Inventory/ObsidianGrid.h"

// ~ Core

// ~ Project

void UObsidianStashTabWidget_Grid::InitializeStashTab(UObsidianInventoryItemsWidgetController* InventoryItemsWidgetController, const int32 GridWidth, const int32 GridHeight)
{
	if(StashTabGrid && InventoryItemsWidgetController)
	{
		StashTabGrid->ConstructGrid(InventoryItemsWidgetController, EObsidianGridOwner::GO_PlayerStash, GridWidth, GridHeight);
	}
}
