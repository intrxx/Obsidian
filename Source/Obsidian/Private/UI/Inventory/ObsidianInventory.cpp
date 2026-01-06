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
}

void UObsidianInventory::NativeConstruct()
{
	Super::NativeConstruct();

	if(InventoryGrid && InventoryItemsWidgetController)
	{
		InventoryGrid->SetWidgetController(InventoryItemsWidgetController);
		InventoryGrid->ConstructInventoryGrid();
	}
	
	if(EquipmentPanel)
	{
		EquipmentPanel->SetWidgetController(InventoryItemsWidgetController);
	}
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
	if(InventoryGrid == nullptr)
	{
		UE_LOG(LogObsidian, Error, TEXT("InventoryGrid is invalid in [%hs]"), __FUNCTION__);
		return;
	}
	
	const FIntPoint DesiredPosition = ItemWidgetData.ItemPosition.GetItemGridPosition();
	const FIntPoint GridSpan = ItemWidgetData.GridSpan;
	
	checkf(ItemWidgetClass, TEXT("Tried to create widget without valid widget class in UObsidianInventory::OnItemAdded,"
							  " fill it in ObsidianInventory instance."));
	UObsidianItem* ItemWidget = CreateWidget<UObsidianItem>(this, ItemWidgetClass);
	ItemWidget->InitializeItemWidget(DesiredPosition, GridSpan, ItemWidgetData.ItemImage, ItemWidgetData.StackCount);
	InventoryGrid->AddItemWidget(ItemWidget, ItemWidgetData);
}

void UObsidianInventory::OnItemEquipped(const FObsidianItemWidgetData& ItemWidgetData)
{
	if(EquipmentPanel == nullptr)
	{
		UE_LOG(LogObsidian, Error, TEXT("EquipmentPanel is invalid in [%hs]"), __FUNCTION__);
		return;
	}
	
	checkf(ItemWidgetClass, TEXT("Tried to create widget without valid widget class in UObsidianInventory::OnItemAdded,"
							  " fill it in ObsidianInventory instance."));
	UObsidianItem* ItemWidget = CreateWidget<UObsidianItem>(this, ItemWidgetClass);
	ItemWidget->InitializeItemWidget(ItemWidgetData.GridSpan, ItemWidgetData.ItemImage, ItemWidgetData.IsItemForSwapSlot());
	EquipmentPanel->AddItemWidget(ItemWidget, ItemWidgetData);

	if (ItemWidgetData.bDoesBlockSisterSlot)
	{
		UObsidianItem* BlockingItemWidget = CreateWidget<UObsidianItem>(this, ItemWidgetClass);
		BlockingItemWidget->InitializeItemWidget(ItemWidgetData.GridSpan, ItemWidgetData.ItemImage, ItemWidgetData.IsItemForSwapSlot());
		EquipmentPanel->AddItemWidget(BlockingItemWidget, ItemWidgetData, true);
	}
}

void UObsidianInventory::OnItemUnequipped(const FGameplayTag& SlotTag, const bool bBlocksOtherSlot)
{
	if (ensure(EquipmentPanel))
	{
		EquipmentPanel->HandleItemUnequipped(SlotTag, bBlocksOtherSlot);
	}
}

void UObsidianInventory::OnItemChanged(const FObsidianItemWidgetData& ItemWidgetData)
{
	if (ensure(ItemWidgetData.ItemPosition.IsOnInventoryGrid()))
	{
		InventoryGrid->HandleItemStackChanged(ItemWidgetData.ItemPosition.GetItemGridPosition(), ItemWidgetData.StackCount);
	}
}

void UObsidianInventory::OnItemRemoved(const FObsidianItemPosition& FromPosition)
{
	if (ensure(InventoryGrid && FromPosition.IsOnInventoryGrid()))
	{
		InventoryGrid->HandleItemRemoved(FromPosition);
	}
}

void UObsidianInventory::HighlightSlotPlacement(const FGameplayTagContainer& WithTags)
{
	for (UObsidianItemSlot_Equipment* SlotWidget : EquipmentPanel->GetAllSlots())
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


