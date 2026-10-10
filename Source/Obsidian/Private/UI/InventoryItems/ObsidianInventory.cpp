// Copyright 2026 out of sCope team - intrxx

#include "UI/InventoryItems/ObsidianInventory.h"

#include "Characters/Player/ObsidianPlayerController.h"
#include "InventoryItems/Crafting/ObsidianCraftingComponent.h"
#include "Obsidian/ObsidianLogCategories.h"
#include "UI/InventoryItems/Items/ObsidianItem.h"
#include "UI/InventoryItems/ObsidianGridPanel.h"
#include "UI/InventoryItems/ObsidianSlotPanel.h"
#include "UI/InventoryItems/Slots/ObsidianSlot_ItemSlot.h"
#include "UI/InventoryItems/Slots/ObsidianSlotBase.h"
#include "UI/WidgetControllers/ObInventoryItemsWidgetController.h"


void UObsidianInventory::HandleWidgetControllerSet()
{
	InventoryItemsWidgetController = Cast<UObInventoryItemsWidgetController>(WidgetController);
	check(InventoryItemsWidgetController);

	InventoryItemsWidgetController->OnItemEquippedDelegate.AddUObject(this, &ThisClass::OnEquipmentItemAdded);
	InventoryItemsWidgetController->OnEquippedItemChangedDelegate.AddUObject(this, &ThisClass::OnEquipmentItemChanged);
	InventoryItemsWidgetController->OnEquippedItemRemovedDelegate.AddUObject(this, &ThisClass::OnEquipmentItemRemoved);
	
	InventoryItemsWidgetController->OnItemInventorizedDelegate.AddUObject(this, &ThisClass::OnInventoryItemAdded);
	InventoryItemsWidgetController->OnInventoryItemChangedDelegate.AddUObject(this, &ThisClass::OnInventoryItemChanged);
	InventoryItemsWidgetController->OnInventorizedItemRemovedDelegate.AddUObject(this, &ThisClass::OnInventoryItemRemoved);

	InventoryItemsWidgetController->OnUsableContextFiredForInventoryDelegate.AddUObject(this, &ThisClass::OnUsableContextFiredForInventory);
	InventoryItemsWidgetController->OnUsableContextFiredForEquipmentDelegate.AddUObject(this, &ThisClass::OnUsableContextFiredForEquipment);
	
	InventoryItemsWidgetController->OnStartPlacementHighlightDelegate.AddUObject(this, &ThisClass::HighlightSlotPlacement);
	InventoryItemsWidgetController->OnStopPlacementHighlightDelegate.AddUObject(this, &ThisClass::StopHighlightSlotPlacement);

	const AObsidianPlayerController* PlayerController = InventoryItemsWidgetController->GetOwningPlayerController();
	if (PlayerController == nullptr)
	{
		UE_LOG(ObLogUIItems, Error, TEXT("PlayerController is invalid in [%hs]."), __FUNCTION__);
	}
	
	if (UObsidianCraftingComponent* CraftingComp = PlayerController->GetCraftingComponent())
	{
		CraftingComp->OnStopUsingItemDelegate.AddUObject(this, &ThisClass::ClearUsableItemHighlight);
	}

	if(Inventory_GridPanel)
	{
		Inventory_GridPanel->SetWidgetController(InventoryItemsWidgetController);
		const bool bSuccess = Inventory_GridPanel->ConstructInventoryPanel();
		ensureMsgf(bSuccess, TEXT("Inventory_GridPanel was unable to construct the InventoryGrid!"));
	}
	
	if(Equipment_SlotPanel)
	{
		Equipment_SlotPanel->SetWidgetController(InventoryItemsWidgetController);
		const bool bSuccess = Equipment_SlotPanel->ConstructEquipmentPanel();
		ensureMsgf(bSuccess, TEXT("Equipment_SlotPanel was unable to construct the EquipmentSlots!"));
	}
}

void UObsidianInventory::NativeDestruct()
{
	if(InventoryItemsWidgetController)
	{
		InventoryItemsWidgetController->RemoveItemUIElements(EObsidianPanelOwner::Inventory);
		InventoryItemsWidgetController->OnItemEquippedDelegate.Clear();
		InventoryItemsWidgetController->OnEquippedItemRemovedDelegate.Clear();
		
		InventoryItemsWidgetController->OnItemInventorizedDelegate.Clear();
		InventoryItemsWidgetController->OnInventoryItemChangedDelegate.Clear();
		InventoryItemsWidgetController->OnInventorizedItemRemovedDelegate.Clear();

		InventoryItemsWidgetController->OnStartPlacementHighlightDelegate.Clear();
		InventoryItemsWidgetController->OnStopPlacementHighlightDelegate.Clear();
	}
	
	Super::NativeDestruct();
}

void UObsidianInventory::OnInventoryItemAdded(const FObsidianItemWidgetData& InItemWidgetData)
{
	if(Inventory_GridPanel == nullptr)
	{
		UE_LOG(ObLogItems, Error, TEXT("Inventory_GridPanel is invalid in [%hs]"), __FUNCTION__);
		return;
	}
	
	const FIntPoint DesiredPosition = InItemWidgetData.ItemPosition.GetItemGridPosition();
	const FIntPoint GridSpan = InItemWidgetData.GridSpan;
	
	checkf(ItemWidgetClass, TEXT("Tried to create widget without valid widget class in UObsidianInventory::OnInventoryItemAdded,"
							  " fill it in ObsidianInventory instance."));
	UObsidianItem* ItemWidget = CreateWidget<UObsidianItem>(this, ItemWidgetClass);
	ItemWidget->InitializeItemWidget(GridSpan, InItemWidgetData.ItemImage, InItemWidgetData.StackCount);
	Inventory_GridPanel->AddItemWidget(ItemWidget, InItemWidgetData);
}

void UObsidianInventory::OnInventoryItemChanged(const FObsidianItemWidgetData& InItemWidgetData)
{
	if (ensure(InItemWidgetData.ItemPosition.IsOnInventoryGrid()))
	{
		Inventory_GridPanel->HandleItemChanged(InItemWidgetData);
	}
}

void UObsidianInventory::OnInventoryItemRemoved(const FObsidianItemWidgetData& InItemWidgetData)
{
	if (ensure(Inventory_GridPanel && InItemWidgetData.ItemPosition.IsOnInventoryGrid()))
	{
		Inventory_GridPanel->HandleItemRemoved(InItemWidgetData);
	}
}

void UObsidianInventory::OnEquipmentItemAdded(const FObsidianItemWidgetData& InItemWidgetData)
{
	if(Equipment_SlotPanel == nullptr)
	{
		UE_LOG(ObLogItems, Error, TEXT("Equipment_SlotPanel is invalid in [%hs]"), __FUNCTION__);
		return;
	}
	
	checkf(ItemWidgetClass, TEXT("Tried to create widget without valid widget class in UObsidianInventory::OnInventoryItemAdded,"
							  " fill it in ObsidianInventory instance."));
	UObsidianItem* ItemWidget = CreateWidget<UObsidianItem>(this, ItemWidgetClass);
	ItemWidget->InitializeItemWidget(InItemWidgetData.GridSpan, InItemWidgetData.ItemImage,
		InItemWidgetData.IsItemForSwapSlot());
	Equipment_SlotPanel->AddItemWidget(ItemWidget, InItemWidgetData);

	if (InItemWidgetData.bDoesBlockSisterSlot)
	{
		UObsidianItem* BlockingItemWidget = CreateWidget<UObsidianItem>(this, ItemWidgetClass);
		BlockingItemWidget->InitializeItemWidget(InItemWidgetData.GridSpan, InItemWidgetData.ItemImage,
			InItemWidgetData.IsItemForSwapSlot());
		Equipment_SlotPanel->AddItemWidget(BlockingItemWidget, InItemWidgetData, true);
	}
}

void UObsidianInventory::OnEquipmentItemChanged(const FObsidianItemWidgetData& InItemWidgetData)
{
	if (ensure(Equipment_SlotPanel))
	{
		Equipment_SlotPanel->HandleItemChanged(InItemWidgetData);
	}
}

void UObsidianInventory::OnEquipmentItemRemoved(const FObsidianItemWidgetData& InItemWidgetData)
{
	if (ensure(Equipment_SlotPanel))
	{
		Equipment_SlotPanel->HandleItemRemoved(InItemWidgetData);
	}
}

void UObsidianInventory::OnUsableContextFiredForInventory(const TArray<FObsidianItemPosition>& InMatchingItemPositions)
{
	if (ensure(Inventory_GridPanel))
	{
		Inventory_GridPanel->HandleHighlightingItems(InMatchingItemPositions);
	}
}

void UObsidianInventory::OnUsableContextFiredForEquipment(const TArray<FObsidianItemPosition>& InMatchingItemPositions)
{
	if (ensure(Equipment_SlotPanel))
	{
		Equipment_SlotPanel->HandleHighlightingItems(InMatchingItemPositions);
	}
}

void UObsidianInventory::ClearUsableItemHighlight()
{
	if (ensure(Equipment_SlotPanel) && ensure(Inventory_GridPanel))
	{
		Equipment_SlotPanel->ClearUsableItemHighlight();
		Inventory_GridPanel->ClearUsableItemHighlight();
	}
}

void UObsidianInventory::HighlightSlotPlacement(const FGameplayTagContainer& InWithTags)
{
	for (UObsidianSlot_ItemSlot* SlotWidget : Equipment_SlotPanel->GetAllSlots())
	{
		if (SlotWidget && InWithTags.HasTagExact(SlotWidget->GetSlotTag()))
		{
			SlotWidget->SetSlotState(EObsidianItemSlotState::GreenLight, EObsidianItemSlotStatePriority::High);
			CachedHighlightedSlot.Add(SlotWidget);
		}
	}
}

void UObsidianInventory::StopHighlightSlotPlacement()
{
	for (UObsidianSlot_ItemSlot* SlotWidget : CachedHighlightedSlot)
	{
		if (SlotWidget)
		{
			SlotWidget->SetSlotState(EObsidianItemSlotState::Neutral, EObsidianItemSlotStatePriority::High);
		}
	}
	
	CachedHighlightedSlot.Empty();
}


