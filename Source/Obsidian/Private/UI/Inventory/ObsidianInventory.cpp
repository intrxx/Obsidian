// Copyright 2026 out of sCope team - intrxx

#include "UI/Inventory/ObsidianInventory.h"

#include "Obsidian/ObsidianGameModule.h"
#include "UI/Inventory/Slots/ObsidianSlotBase.h"
#include "UI/Inventory/Items/ObsidianItem.h"
#include "UI/Inventory/Slots/ObsidianSlot_ItemSlot.h"
#include "UI/WidgetControllers/ObInventoryItemsWidgetController.h"
#include "UI/Inventory/ObsidianSlotPanel.h"
#include "UI/Inventory/ObsidianGridPanel.h"

void UObsidianInventory::HandleWidgetControllerSet()
{
	InventoryItemsWidgetController = Cast<UObInventoryItemsWidgetController>(WidgetController);
	check(InventoryItemsWidgetController);

	InventoryItemsWidgetController->OnItemEquippedDelegate.AddUObject(this, &ThisClass::OnEquipmentItemAdded);
	InventoryItemsWidgetController->OnEquippedItemChangedDelegate.AddUObject(this, &ThisClass::OnEquipmentItemRemoved);
	InventoryItemsWidgetController->OnEquippedItemRemovedDelegate.AddUObject(this, &ThisClass::OnEquipmentItemRemoved);
	
	InventoryItemsWidgetController->OnItemInventorizedDelegate.AddUObject(this, &ThisClass::OnInventoryItemAdded);
	InventoryItemsWidgetController->OnInventoryItemChangedDelegate.AddUObject(this, &ThisClass::OnInventoryItemChanged);
	InventoryItemsWidgetController->OnInventorizedItemRemovedDelegate.AddUObject(this, &ThisClass::OnInventoryItemRemoved);
	
	InventoryItemsWidgetController->OnStartPlacementHighlightDelegate.AddUObject(this, &ThisClass::HighlightSlotPlacement);
	InventoryItemsWidgetController->OnStopPlacementHighlightDelegate.AddUObject(this, &ThisClass::StopHighlightSlotPlacement);

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

void UObsidianInventory::OnInventoryItemAdded(const FObsidianItemWidgetData& ItemWidgetData)
{
	if(Inventory_GridPanel == nullptr)
	{
		UE_LOG(LogObsidian, Error, TEXT("Inventory_GridPanel is invalid in [%hs]"), __FUNCTION__);
		return;
	}
	
	const FIntPoint DesiredPosition = ItemWidgetData.ItemPosition.GetItemGridPosition();
	const FIntPoint GridSpan = ItemWidgetData.GridSpan;
	
	checkf(ItemWidgetClass, TEXT("Tried to create widget without valid widget class in UObsidianInventory::OnInventoryItemAdded,"
							  " fill it in ObsidianInventory instance."));
	UObsidianItem* ItemWidget = CreateWidget<UObsidianItem>(this, ItemWidgetClass);
	ItemWidget->InitializeItemWidget(GridSpan, ItemWidgetData.ItemImage, ItemWidgetData.StackCount);
	Inventory_GridPanel->AddItemWidget(ItemWidget, ItemWidgetData);
}

void UObsidianInventory::OnInventoryItemChanged(const FObsidianItemWidgetData& ItemWidgetData)
{
	if (ensure(ItemWidgetData.ItemPosition.IsOnInventoryGrid()))
	{
		Inventory_GridPanel->HandleItemChanged(ItemWidgetData);
	}
}

void UObsidianInventory::OnInventoryItemRemoved(const FObsidianItemWidgetData& ItemWidgetData)
{
	if (ensure(Inventory_GridPanel && ItemWidgetData.ItemPosition.IsOnInventoryGrid()))
	{
		Inventory_GridPanel->HandleItemRemoved(ItemWidgetData);
	}
}

void UObsidianInventory::OnEquipmentItemAdded(const FObsidianItemWidgetData& ItemWidgetData)
{
	if(Equipment_SlotPanel == nullptr)
	{
		UE_LOG(LogObsidian, Error, TEXT("Equipment_SlotPanel is invalid in [%hs]"), __FUNCTION__);
		return;
	}
	
	checkf(ItemWidgetClass, TEXT("Tried to create widget without valid widget class in UObsidianInventory::OnInventoryItemAdded,"
							  " fill it in ObsidianInventory instance."));
	UObsidianItem* ItemWidget = CreateWidget<UObsidianItem>(this, ItemWidgetClass);
	ItemWidget->InitializeItemWidget(ItemWidgetData.GridSpan, ItemWidgetData.ItemImage,
		ItemWidgetData.IsItemForSwapSlot());
	Equipment_SlotPanel->AddItemWidget(ItemWidget, ItemWidgetData);

	if (ItemWidgetData.bDoesBlockSisterSlot)
	{
		UObsidianItem* BlockingItemWidget = CreateWidget<UObsidianItem>(this, ItemWidgetClass);
		BlockingItemWidget->InitializeItemWidget(ItemWidgetData.GridSpan, ItemWidgetData.ItemImage,
			ItemWidgetData.IsItemForSwapSlot());
		Equipment_SlotPanel->AddItemWidget(BlockingItemWidget, ItemWidgetData, true);
	}
}

void UObsidianInventory::OnEquipmentItemChanged(const FObsidianItemWidgetData& ItemWidgetData)
{
	if (ensure(Equipment_SlotPanel))
	{
		Equipment_SlotPanel->HandleItemChanged(ItemWidgetData);
	}
}

void UObsidianInventory::OnEquipmentItemRemoved(const FObsidianItemWidgetData& ItemWidgetData)
{
	if (ensure(Equipment_SlotPanel))
	{
		Equipment_SlotPanel->HandleItemRemoved(ItemWidgetData);
	}
}

void UObsidianInventory::HighlightSlotPlacement(const FGameplayTagContainer& WithTags)
{
	for (UObsidianSlot_ItemSlot* SlotWidget : Equipment_SlotPanel->GetAllSlots())
	{
		if (SlotWidget && WithTags.HasTagExact(SlotWidget->GetSlotTag()))
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


