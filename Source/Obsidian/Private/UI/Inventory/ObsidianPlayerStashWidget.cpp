// Copyright 2024 out of sCope team - Michał Ogiński

#include "UI/Inventory/ObsidianPlayerStashWidget.h"

// ~ Core
#include "Components/Overlay.h"

// ~ Project
#include "Components/ScrollBox.h"
#include "InventoryItems/PlayerStash/ObsidianStashTabsConfig.h"
#include "UI/WidgetControllers/ObsidianInventoryItemsWidgetController.h"
#include "InventoryItems/PlayerStash/ObsidianStashTab.h"
#include "InventoryItems/PlayerStash/Tabs/ObsidianStashTab_Grid.h"
#include "UI/Inventory/Stash/ObsidianStashTabWidget_Grid.h"
#include "UI/Inventory/Stash/ObsidianStashTabWidget_Slots.h"
#include "UI/Inventory/Stash/ObsidianStashButton.h"

void UObsidianPlayerStashWidget::HandleWidgetControllerSet()
{
	Super::HandleWidgetControllerSet();
	
	InventoryItemsWidgetController = Cast<UObsidianInventoryItemsWidgetController>(WidgetController);
	check(InventoryItemsWidgetController);
	
}

void UObsidianPlayerStashWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	CreateStashTabs();
}

void UObsidianPlayerStashWidget::CreateStashTabButton(const FGameplayTag& StashTag, const FText& StashTabName)
{
	if(StashTag.IsValid() == false)
	{
		UE_LOG(LogWidgetController_Items, Error, TEXT("Trying to create Stash Tab Button without valid Stash Tag in [%hs]"), ANSI_TO_TCHAR(__FUNCTION__));
		return;
	}

	checkf(StashButtonWidgetClass, TEXT("Trying to create Stash Button without valid StashButtonWidgetClass."))
	UObsidianStashButton* StashButton = CreateWidget<UObsidianStashButton>(this, StashButtonWidgetClass);
	StashButton->InitializeStashButton(StashTag, StashTabName);
	StashButton->OnStashTabButtonPressedDelegate.AddUObject(this, &ThisClass::ShowStashTab);
	
	StashTabsList_ScrollBox->AddChild(StashButton);
}

void UObsidianPlayerStashWidget::ShowStashTab(const FGameplayTag& WithStashTag)
{
	if(WithStashTag.IsValid() == false)
	{
		UE_LOG(LogWidgetController_Items, Error, TEXT("Trying to show Stash Tab with invalid tag in [%hs]."), ANSI_TO_TCHAR(__FUNCTION__));
		return;
	}
	
	if(UObsidianStashTabWidget* StashTabToShow = *StashTabsMap.Find(WithStashTag))
	{
		if(ActiveStashTab && ActiveStashTab->IsTabActive())
		{
			if(StashTabToShow == ActiveStashTab)
			{
				UE_LOG(LogWidgetController_Items, Warning, TEXT("Trying to show the same tab, should disable the button maybe?"));
				return;
			}
			
			ActiveStashTab->HideStashTab();
		}
		
		StashTabToShow->ShowStashTab();
		ActiveStashTab = StashTabToShow;
	}
}

void UObsidianPlayerStashWidget::CreateStashTabs()
{
	const UObsidianStashTabsConfig* StashConfig = InventoryItemsWidgetController->GetStashTabConfig();
	const TArray<FObsidianStashTabDefinition> StashTabDefinitions = StashConfig->GetStashTabDefinitions();
	StashTabsMap.Empty(StashTabDefinitions.Num());

	int32 StashTabCounter = 1;
	
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
				StashTabsMap.Add(Definition.StashTag, GridStashTabWidget);
				
				CreateStashTabButton(Definition.StashTag, FText::FromString(FString::Printf(TEXT("%d"), StashTabCounter)));
				StashTabCounter++;
			}
		}
		else if(Definition.StashTabType == EObsidianStashTabType::STT_SlotType)
		{
			if(Definition.StashTabClass == nullptr)
			{
				return;
			}
			
			checkf(Definition.StashTabWidgetClass, TEXT("Trying to create Grid Stash Tab without valid Widget Class"));
			UObsidianStashTabWidget_Slots* SlotStashTabWidget = CreateWidget<UObsidianStashTabWidget_Slots>(this, Definition.StashTabWidgetClass);
			SlotStashTabWidget->InitializeStashTab(this);

			StashTab_Overlay->AddChildToOverlay(SlotStashTabWidget);
			StashTabsMap.Add(Definition.StashTag, SlotStashTabWidget);

			CreateStashTabButton(Definition.StashTag, FText::FromString(FString::Printf(TEXT("%d"), StashTabCounter)));
			StashTabCounter++;
		}
	}

	//TODO This is not the best (but at least in Obsidian this will be the case),
	// but since I can't think of not an ugly way of getting the first Stash Tab right now this will be here for a little bit
	ShowStashTab(ObsidianGameplayTags::StashTab_Grid_1);
}
