// Copyright 2024 out of sCope team - Michał Ogiński

#include "UI/Inventory/ObsidianPlayerStashWidget.h"

// ~ Core

// ~ Project
#include "Components/Overlay.h"
#include "InventoryItems/PlayerStash/ObsidianStashTabsConfig.h"
#include "UI/WidgetControllers/ObsidianInventoryItemsWidgetController.h"
#include "InventoryItems/PlayerStash/ObsidianStashTab.h"
#include "InventoryItems/PlayerStash/Tabs/ObsidianStashTab_Grid.h"
#include "UI/Inventory/Stash/ObsidianStashTabWidget_Grid.h"
#include "UI/Inventory/Stash/ObsidianStashTabWidget_Slots.h"

void UObsidianPlayerStashWidget::HandleWidgetControllerSet()
{
	Super::HandleWidgetControllerSet();
	
	InventoryItemsWidgetController = Cast<UObsidianInventoryItemsWidgetController>(WidgetController);
	check(InventoryItemsWidgetController);
	
}

void UObsidianPlayerStashWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	const UObsidianStashTabsConfig* StashConfig = InventoryItemsWidgetController->GetStashTabConfig();
	const TArray<FObsidianStashTabDefinition> StashTabDefinitions = StashConfig->GetStashTabDefinitions();
	StashTabs.Empty(StashTabDefinitions.Num());
	
	for(const FObsidianStashTabDefinition& Definition : StashTabDefinitions)
	{
		if(Definition.StashTabType == EObsidianStashTabType::STT_GridType)
		{
			if(Definition.StashTabClass == nullptr)
			{
				continue;
			}

			if(const UObsidianStashTab_Grid* GridStashTab = GetDefault<UObsidianStashTab_Grid>(Definition.StashTabClass))
			{
				checkf(Definition.StashTabWidgetClass, TEXT("Trying to create Grid Stash Tab without valid Widget Class"));
				UObsidianStashTabWidget_Grid* GridStashTabWidget = CreateWidget<UObsidianStashTabWidget_Grid>(this, Definition.StashTabWidgetClass);
				GridStashTabWidget->InitializeStashTab(InventoryItemsWidgetController, GridStashTab->GetGridWidth(), GridStashTab->GetGridHeight());

				StashTab_Overlay->AddChildToOverlay(GridStashTabWidget);
				StashTabs.Add(GridStashTabWidget); //TODO This might be a map stash to button
			}
		}
		else if(Definition.StashTabType == EObsidianStashTabType::STT_SlotType)
		{
			checkf(Definition.StashTabWidgetClass, TEXT("Trying to create Grid Stash Tab without valid Widget Class"));
			UObsidianStashTabWidget_Slots* SlotStashTabWidget = CreateWidget<UObsidianStashTabWidget_Slots>(this, Definition.StashTabWidgetClass);
			SlotStashTabWidget->InitializeStashTab(this);

			StashTab_Overlay->AddChildToOverlay(SlotStashTabWidget);
			StashTabs.Add(SlotStashTabWidget); //TODO This might be a map stash to button
		}
	}

	if(StashTabs.IsEmpty() == false)
	{
		StashTabs[5]->ShowStashTab();
	}
}
