// Copyright 2024 out of sCope team - Michał Ogiński

#include "UI/Inventory/ObsidianInventory.h"

#include "Obsidian/ObsidianGameModule.h"
#include "UI/Inventory/Slots/ObsidianItemSlot.h"
#include "UI/Inventory/Items/ObsidianItem.h"
#include "UI/Inventory/Slots/ObsidianItemSlot_Equipment.h"
#include "UI/WidgetControllers/ObInventoryItemsWidgetController.h"
#include "UI/Inventory/ObsidianSlotPanel.h"
#include "UI/Inventory/ObsidianGridPanel.h"

void UObsidianInventory::HandleWidgetControllerSet()
{
	InventoryItemsWidgetController = Cast<UObInventoryItemsWidgetController>(WidgetController);
	check(InventoryItemsWidgetController);

	InventoryItemsWidgetController->OnItemEquippedDelegate.AddUObject(this, &ThisClass::OnItemEquipped);
	InventoryItemsWidgetController->OnEquippedItemRemovedDelegate.AddUObject(this, &ThisClass::OnItemUnequipped);
	
	InventoryItemsWidgetController->OnItemAddedDelegate.AddUObject(this, &ThisClass::OnItemAdded);
	InventoryItemsWidgetController->OnInventoryItemChangedDelegate.AddUObject(this, &ThisClass::OnItemChanged);
	InventoryItemsWidgetController->OnInventorizedItemRemovedDelegate.AddUObject(this, &ThisClass::OnItemRemoved);
	
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

void UObsidianInventory::NativeConstruct()
{
	Super::NativeConstruct();
	
}

void UObsidianInventory::NativeDestruct()
{
	if(InventoryItemsWidgetController)
	{
		InventoryItemsWidgetController->RemoveItemUIElements();
		InventoryItemsWidgetController->OnItemEquippedDelegate.Clear();
		InventoryItemsWidgetController->OnEquippedItemRemovedDelegate.Clear();
		
		InventoryItemsWidgetController->OnItemAddedDelegate.Clear();
		InventoryItemsWidgetController->OnInventoryItemChangedDelegate.Clear();
		InventoryItemsWidgetController->OnInventorizedItemRemovedDelegate.Clear();

		InventoryItemsWidgetController->OnStartPlacementHighlightDelegate.Clear();
		InventoryItemsWidgetController->OnStopPlacementHighlightDelegate.Clear();
	}
	
	Super::NativeDestruct();
}

bool UObsidianInventory::IsPlayerDraggingItem() const
{
	if(InventoryItemsWidgetController)
	{
		return InventoryItemsWidgetController->IsDraggingAnItem();
	}
	return false;
}

bool UObsidianInventory::CanEquipDraggedItem(const FGameplayTag& ToSlotTag) const
{
	if(InventoryItemsWidgetController)
	{
		return InventoryItemsWidgetController->CanEquipDraggedItem(ToSlotTag);
	}
	return false;
}

bool UObsidianInventory::CanInteractWithEquipment() const
{
	if(InventoryItemsWidgetController)
	{
		return InventoryItemsWidgetController->CanInteractWithEquipment();
	}
	return false;
}

void UObsidianInventory::OnItemAdded(const FObsidianItemWidgetData& ItemWidgetData)
{
	if(Inventory_GridPanel == nullptr)
	{
		UE_LOG(LogObsidian, Error, TEXT("Inventory_GridPanel is invalid in [%hs]"), __FUNCTION__);
		return;
	}
	
	const FIntPoint DesiredPosition = ItemWidgetData.ItemPosition.GetItemGridPosition();
	const FIntPoint GridSpan = ItemWidgetData.GridSpan;
	
	checkf(ItemWidgetClass, TEXT("Tried to create widget without valid widget class in UObsidianInventory::OnItemAdded,"
							  " fill it in ObsidianInventory instance."));
	UObsidianItem* ItemWidget = CreateWidget<UObsidianItem>(this, ItemWidgetClass);
	ItemWidget->InitializeItemWidget(DesiredPosition, GridSpan, ItemWidgetData.ItemImage, ItemWidgetData.StackCount);
	Inventory_GridPanel->AddItemWidget(ItemWidget, ItemWidgetData);
}

void UObsidianInventory::OnItemEquipped(const FObsidianItemWidgetData& ItemWidgetData)
{
	if(Equipment_SlotPanel == nullptr)
	{
		UE_LOG(LogObsidian, Error, TEXT("Equipment_SlotPanel is invalid in [%hs]"), __FUNCTION__);
		return;
	}
	
	checkf(ItemWidgetClass, TEXT("Tried to create widget without valid widget class in UObsidianInventory::OnItemAdded,"
							  " fill it in ObsidianInventory instance."));
	UObsidianItem* ItemWidget = CreateWidget<UObsidianItem>(this, ItemWidgetClass);
	ItemWidget->InitializeItemWidget(ItemWidgetData.GridSpan, ItemWidgetData.ItemImage, ItemWidgetData.IsItemForSwapSlot());
	Equipment_SlotPanel->AddItemWidget(ItemWidget, ItemWidgetData);

	if (ItemWidgetData.bDoesBlockSisterSlot)
	{
		UObsidianItem* BlockingItemWidget = CreateWidget<UObsidianItem>(this, ItemWidgetClass);
		BlockingItemWidget->InitializeItemWidget(ItemWidgetData.GridSpan, ItemWidgetData.ItemImage, ItemWidgetData.IsItemForSwapSlot());
		Equipment_SlotPanel->AddItemWidget(BlockingItemWidget, ItemWidgetData, true);
	}
}

void UObsidianInventory::OnItemUnequipped(const FGameplayTag& SlotTag, const bool bBlocksOtherSlot)
{
	if (ensure(Equipment_SlotPanel))
	{
		Equipment_SlotPanel->HandleItemUnequipped(SlotTag, bBlocksOtherSlot);
	}
}

void UObsidianInventory::OnItemChanged(const FObsidianItemWidgetData& ItemWidgetData)
{
	if (ensure(ItemWidgetData.ItemPosition.IsOnInventoryGrid()))
	{
		Inventory_GridPanel->HandleItemStackChanged(ItemWidgetData.ItemPosition.GetItemGridPosition(), ItemWidgetData.StackCount);
	}
}

void UObsidianInventory::OnItemRemoved(const FObsidianItemPosition& FromPosition)
{
	if (ensure(Inventory_GridPanel && FromPosition.IsOnInventoryGrid()))
	{
		Inventory_GridPanel->HandleItemRemoved(FromPosition);
	}
}

void UObsidianInventory::HighlightSlotPlacement(const FGameplayTagContainer& WithTags)
{
	for (UObsidianItemSlot_Equipment* SlotWidget : Equipment_SlotPanel->GetAllSlots())
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
	for (UObsidianItemSlot_Equipment* SlotWidget : CachedHighlightedSlot)
	{
		if (SlotWidget)
		{
			SlotWidget->SetSlotState(EObsidianItemSlotState::Neutral, EObsidianItemSlotStatePriority::High);
		}
	}
	
	CachedHighlightedSlot.Empty();
}


