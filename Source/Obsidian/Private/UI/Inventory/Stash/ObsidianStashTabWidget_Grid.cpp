// Copyright 2024 out of sCope team - Michał Ogiński

#include "UI/Inventory/Stash/ObsidianStashTabWidget_Grid.h"

#include "UI/Inventory/ObsidianGridPanel.h"
#include "UI/WidgetControllers/ObInventoryItemsWidgetController.h"

// ~ Core

// ~ Project

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
		//StashTab_GridPanel->OnGridSlotPressedDelegate.AddUObject(this, &ThisClass::RequestAddingItemToStashTab);
	}
}

void UObsidianStashTabWidget_Grid::AddItemToStash(UObsidianItem* InItemWidget, const float ItemSlotPadding)
{
	if (StashTab_GridPanel)
	{
		StashTab_GridPanel->AddItemWidget(InItemWidget, FObsidianItemWidgetData() /** ItemSlotPadding */);
	}
}

void UObsidianStashTabWidget_Grid::RequestAddingItemToStashTab(const FIntPoint& ToPosition, const bool bShiftDown) const
{
	if (InventoryItemsController)
	{
		InventoryItemsController->RequestAddingItemToStashTab(FObsidianItemPosition(ToPosition, StashTabTag), bShiftDown);
	}
}
