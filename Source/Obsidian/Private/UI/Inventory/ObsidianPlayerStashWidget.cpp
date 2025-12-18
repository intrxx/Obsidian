// Copyright 2024 out of sCope team - Michał Ogiński

#include "UI/Inventory/ObsidianPlayerStashWidget.h"

// ~ Core
#include "Components/Overlay.h"
#include "Components/ScrollBox.h"

// ~ Project
#include "Characters/Player/ObsidianPlayerController.h"
#include "UI/Inventory/Items/ObsidianItem.h"
#include "InventoryItems/PlayerStash/ObsidianStashTabsConfig.h"
#include "UI/WidgetControllers/ObInventoryItemsWidgetController.h"
#include "InventoryItems/PlayerStash/ObsidianStashTab.h"
#include "InventoryItems/PlayerStash/Tabs/ObsidianStashTab_Grid.h"
#include "InventoryItems/PlayerStash/Tabs/ObsidianStashTab_Slots.h"
#include "UI/Inventory/Slots/ObsidianItemSlot_Equipment.h"
#include "UI/Inventory/Stash/ObsidianStashTabWidget_Grid.h"
#include "UI/Inventory/Stash/ObsidianStashTabWidget_Slots.h"
#include "UI/Inventory/Stash/ObsidianStashButton.h"

void UObsidianPlayerStashWidget::HandleWidgetControllerSet()
{
	Super::HandleWidgetControllerSet();
	
	InventoryItemsWidgetController = Cast<UObInventoryItemsWidgetController>(WidgetController);
	check(InventoryItemsWidgetController);

	InventoryItemsWidgetController->OnItemStashedDelegate.AddUObject(this, &ThisClass::OnItemStashed);
	InventoryItemsWidgetController->OnStashedItemChangedDelegate.AddUObject(this, &ThisClass::OnItemChanged);
	InventoryItemsWidgetController->OnStartPlacementHighlightDelegate.AddUObject(this, &ThisClass::HighlightSlotPlacement);
	InventoryItemsWidgetController->OnStopPlacementHighlightDelegate.AddUObject(this, &ThisClass::StopHighlightSlotPlacement);
}

void UObsidianPlayerStashWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	CreateStashTabs();
}

void UObsidianPlayerStashWidget::NativeDestruct()
{
	if(InventoryItemsWidgetController)
	{
		InventoryItemsWidgetController->RemoveItemUIElements();
		InventoryItemsWidgetController->OnItemStashedDelegate.Clear();
	}
	
	Super::NativeDestruct();
}

UObsidianStashTabWidget* UObsidianPlayerStashWidget::GetActiveStashTab() const
{
	return ActiveStashTab;
}

FGameplayTag UObsidianPlayerStashWidget::GetActiveStashTabTag() const
{
	if(ActiveStashTab)
	{
		return ActiveStashTab->GetStashTabTag();
	}
	return FGameplayTag::EmptyTag;
}

void UObsidianPlayerStashWidget::HighlightSlotPlacement(const FGameplayTagContainer& WithTags)
{
	if(const UObsidianStashTabWidget_Slots* StashTabWidget_Slots = Cast<UObsidianStashTabWidget_Slots>(ActiveStashTab))
	{
		for (UObsidianItemSlot_Equipment* SlotWidget : StashTabWidget_Slots->GetSlotWidgets())
		{
			if (SlotWidget && WithTags.HasTagExact(SlotWidget->GetSlotTag()))
			{
				SlotWidget->SetSlotState(EObsidianItemSlotState::GreenLight, EObsidianItemSlotStatePriority::High);
				CachedHighlightedSlot.Add(SlotWidget);
			}
		}
	}
}

void UObsidianPlayerStashWidget::StopHighlightSlotPlacement()
{
	for (UObsidianItemSlot_Equipment* SlotWidget : CachedHighlightedSlot)
	{
		if (SlotWidget)
		{
			SlotWidget->SetSlotState(EObsidianItemSlotState::Neutral, EObsidianItemSlotStatePriority::High);
		}
	}
	
	CachedHighlightedSlot.Empty();
}

void UObsidianPlayerStashWidget::CloseStash()
{
	ActiveStashTab->HideStashTab();
	ActiveStashTab = nullptr;
	RemoveFromParent();
}

void UObsidianPlayerStashWidget::OnItemStashed(const FObsidianItemWidgetData& ItemWidgetData)
{
	const FGameplayTag StashTabTag = ItemWidgetData.ItemPosition.GetOwningStashTabTag();
	if (UObsidianStashTabWidget** StashTabWidgetPointer = StashTabsMap.Find(StashTabTag))
	{
		UObsidianStashTabWidget* StashTabWidget = *StashTabWidgetPointer;
		
		const FIntPoint GridSpan = ItemWidgetData.GridSpan;
	
		checkf(ItemWidgetClass, TEXT("Tried to create widget without valid widget class in UObsidianInventory::OnItemAdded, fill it in ObsidianInventory instance."));
		UObsidianItem* ItemWidget = CreateWidget<UObsidianItem>(this, ItemWidgetClass);
		ItemWidget->InitializeItemWidget(ItemWidgetData.ItemPosition, GridSpan, ItemWidgetData.ItemImage, ItemWidgetData.StackCount);
		ItemWidget->OnItemLeftMouseButtonPressedDelegate.AddUObject(this, &ThisClass::OnStashedItemLeftMouseButtonDown);
		ItemWidget->OnItemMouseEnterDelegate.AddUObject(this, &ThisClass::OnStashedItemMouseEntered);
		ItemWidget->OnItemMouseLeaveDelegate.AddUObject(this, &ThisClass::OnItemMouseLeave);
	
		if(ItemWidgetData.bUsable)
		{
			ItemWidget->OnItemRightMouseButtonPressedDelegate.AddUObject(this, &ThisClass::OnStashedItemRightMouseButtonPressed);
		}
		
		InventoryItemsWidgetController->RegisterStashTabItemWidget(ItemWidgetData.ItemPosition, ItemWidget);
		StashTabWidget->AddItemToStash(ItemWidget, ItemWidgetData.ItemSlotPadding);
	}
}

void UObsidianPlayerStashWidget::OnItemChanged(const FObsidianItemWidgetData& ItemWidgetData) 
{
	if(UObsidianItem* ItemWidget = InventoryItemsWidgetController->GetItemWidgetAtStashPosition(ItemWidgetData.ItemPosition))
	{
		ItemWidget->OverrideCurrentStackCount(ItemWidgetData.StackCount);
	}
}

void UObsidianPlayerStashWidget::OnStashedItemRightMouseButtonPressed(UObsidianItem* ItemWidget)
{
	ensureMsgf(ItemWidget, TEXT("Item Widget is invalid in UObsidianPlayerStashWidget::OnStashedItemRightMouseButtonPressed"));
	if(InventoryItemsWidgetController)
	{
		InventoryItemsWidgetController->HandleRightClickingOnStashedItem(ItemWidget->GetItemPosition(), ItemWidget);
	}
}

void UObsidianPlayerStashWidget::OnStashedItemLeftMouseButtonDown(const UObsidianItem* ItemWidget, const FObsidianItemInteractionFlags& InteractionFlags)
{
	ensureMsgf(ItemWidget, TEXT("Item Widget is invalid in UObsidianPlayerStashWidget::OnStashedItemLeftMouseButtonDown"));

	if(InventoryItemsWidgetController)
	{
		if(InteractionFlags.bItemStacksInteraction)
		{
			InventoryItemsWidgetController->HandleLeftClickingOnStashedItemWithShiftDown(ItemWidget->GetItemPosition(), ItemWidget);
			return;
		}
		InventoryItemsWidgetController->HandleLeftClickingOnStashedItem(ItemWidget->GetItemPosition(), InteractionFlags.bMoveBetweenNextOpenedWindow);
	}
}

void UObsidianPlayerStashWidget::OnStashedItemMouseEntered(const UObsidianItem* ItemWidget)
{
	ensureMsgf(ItemWidget, TEXT("Item Widget is invalid in UObsidianPlayerStashWidget::OnStashedItemMouseEntered"));
	if(InventoryItemsWidgetController)
	{
		InventoryItemsWidgetController->HandleHoveringOverStashedItem(ItemWidget);
	}
}

void UObsidianPlayerStashWidget::OnItemMouseLeave()
{
	if(InventoryItemsWidgetController)
	{
		InventoryItemsWidgetController->HandleUnhoveringItem();
	}
}

void UObsidianPlayerStashWidget::CreateStashTabButton(const FGameplayTag& StashTag, const FText& StashTabName)
{
	if(StashTag.IsValid() == false)
	{
		UE_LOG(LogWidgetController_Items, Error, TEXT("Trying to create Stash Tab Button without valid Stash Tag in [%hs]"), __FUNCTION__);
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
		UE_LOG(LogWidgetController_Items, Error, TEXT("Trying to show Stash Tab with invalid tag in [%hs]."), __FUNCTION__);
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
		InventoryItemsWidgetController->RegisterCurrentStashTab(WithStashTag);
	}
}

void UObsidianPlayerStashWidget::CreateStashTabs()
{
	check(InventoryItemsWidgetController);

	const TConstArrayView<TObjectPtr<UObsidianStashTab>> StashTabs = InventoryItemsWidgetController->GetAllStashTabs();
	StashTabsMap.Empty(StashTabs.Num());

	int32 StashTabCounter = 1;
	
	for(const UObsidianStashTab* Tab : StashTabs)
	{
		if (Tab == nullptr)
		{
			continue;
		}
		
		if(Tab->GetStashTabType() == EObsidianStashTabType::STT_GridType)
		{
			if(const UObsidianStashTab_Grid* GridStashTab = Cast<UObsidianStashTab_Grid>(Tab))
			{
				checkf(Tab->GetWidgetClass(), TEXT("Trying to create Grid Stash Tab without valid Widget Class"));
				UObsidianStashTabWidget_Grid* GridStashTabWidget = CreateWidget<UObsidianStashTabWidget_Grid>(this,
					Tab->GetWidgetClass());

				const FGameplayTag StashTabTag = Tab->GetStashTabTag();
				GridStashTabWidget->InitializeStashTab(InventoryItemsWidgetController, GridStashTab->GetGridWidth(),
					GridStashTab->GetGridHeight(), StashTabTag);

				StashTab_Overlay->AddChildToOverlay(GridStashTabWidget);
				StashTabsMap.Add(StashTabTag, GridStashTabWidget);
				
				CreateStashTabButton(StashTabTag, FText::FromString(Tab->GetStashTabName()));
				StashTabCounter++;
			}
		}
		else if(Tab->GetStashTabType() == EObsidianStashTabType::STT_SlotType)
		{
			checkf(Tab->GetWidgetClass(), TEXT("Trying to create Grid Stash Tab without valid Widget Class"));
			UObsidianStashTabWidget_Slots* SlotStashTabWidget = CreateWidget<UObsidianStashTabWidget_Slots>(this,
				Tab->GetWidgetClass());

			const FGameplayTag StashTabTag = Tab->GetStashTabTag();
			SlotStashTabWidget->InitializeStashTab(InventoryItemsWidgetController, this, StashTabTag);

			StashTab_Overlay->AddChildToOverlay(SlotStashTabWidget);
			StashTabsMap.Add(StashTabTag, SlotStashTabWidget);

			CreateStashTabButton(StashTabTag, FText::FromString(Tab->GetStashTabName()));
			StashTabCounter++;
		}
	}
	
	if (StashTabs.IsEmpty() == false)
	{
		if (const UObsidianStashTab* StashTab = StashTabs[0])
		{
			ShowStashTab(StashTab->GetStashTabTag());
		}
	}
	
}
