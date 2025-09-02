// Copyright 2024 out of sCope team - Michał Ogiński

#include "UI/Inventory/ObsidianInventory.h"

// ~ Core

// ~ Project
#include "Obsidian/Public/UI/Inventory/Slots/ObsidianItemSlot.h"
#include "UI/Inventory/Items/ObsidianItem.h"
#include "UI/Inventory/Slots/ObsidianSlotBlockadeItem.h"
#include "Obsidian/Public/UI/Inventory/Slots/ObsidianItemSlot_Equipment.h"
#include "UI/WidgetControllers/ObsidianInventoryItemsWidgetController.h"
#include "UI/Inventory/ObsidianEquipmentPanel.h"
#include "UI/Inventory/ObsidianGrid.h"

void UObsidianInventory::HandleWidgetControllerSet()
{
	InventoryItemsWidgetController = Cast<UObsidianInventoryItemsWidgetController>(WidgetController);
	check(InventoryItemsWidgetController);

	InventoryItemsWidgetController->OnItemEquippedDelegate.AddUObject(this, &ThisClass::OnItemEquipped);
	
	InventoryItemsWidgetController->OnItemAddedDelegate.AddUObject(this, &ThisClass::OnItemAdded);
	InventoryItemsWidgetController->OnItemChangedDelegate.AddUObject(this, &ThisClass::OnItemChanged);
}

void UObsidianInventory::NativeConstruct()
{
	Super::NativeConstruct();

	if(InventoryGrid && InventoryItemsWidgetController)
	{
		InventoryGrid->ConstructGrid(InventoryItemsWidgetController, EObsidianGridOwner::GO_Inventory, InventoryItemsWidgetController->GetInventoryGridWidth(), InventoryItemsWidgetController->GetInventoryGridHeight());
		InventoryGrid->OnGridSlotPressedDelegate.AddUObject(this, &ThisClass::RequestAddingItemToInventory);
	}
	
	if(EquipmentPanel)
	{
		EquipmentPanel->InitializeEquipmentPanel(this);
	}
}

void UObsidianInventory::NativeDestruct()
{
	if(InventoryItemsWidgetController)
	{
		InventoryItemsWidgetController->RemoveItemUIElements();
		InventoryItemsWidgetController->OnItemEquippedDelegate.Clear();
		InventoryItemsWidgetController->OnItemAddedDelegate.Clear();
		InventoryItemsWidgetController->OnItemChangedDelegate.Clear();
	}
	
	if(InventoryGrid)
	{
		InventoryGrid->OnGridSlotPressedDelegate.Clear();
	}
	
	Super::NativeDestruct();
}

bool UObsidianInventory::IsPlayerDraggingItem() const
{
	if(InventoryItemsWidgetController == false)
	{
		return false;
	}
	return InventoryItemsWidgetController->IsDraggingAnItem();
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
	const FIntPoint DesiredPosition = ItemWidgetData.ItemPosition.GetItemGridLocation();
	const FIntPoint GridSpan = ItemWidgetData.GridSpan;
	
	checkf(ItemWidgetClass, TEXT("Tried to create widget without valid widget class in UObsidianInventory::OnItemAdded, fill it in ObsidianInventory instance."));
	UObsidianItem* ItemWidget = CreateWidget<UObsidianItem>(this, ItemWidgetClass);
	ItemWidget->InitializeItemWidget(DesiredPosition, GridSpan, ItemWidgetData.ItemImage, ItemWidgetData.StackCount);
	ItemWidget->OnItemLeftMouseButtonPressedDelegate.AddUObject(this, &ThisClass::OnInventoryItemLeftMouseButtonPressed);
	ItemWidget->OnItemMouseEnterDelegate.AddUObject(this, &ThisClass::OnInventoryItemMouseEntered);
	ItemWidget->OnItemMouseLeaveDelegate.AddUObject(this, &ThisClass::OnItemMouseLeave);
	
	if(ItemWidgetData.bUsable)
	{
		ItemWidget->OnItemRightMouseButtonPressedDelegate.AddUObject(this, &ThisClass::OnInventoryItemRightMouseButtonPressed);
	}
	
	InventoryItemsWidgetController->RegisterInventoryItemWidget(DesiredPosition, ItemWidget);
	InventoryGrid->AddItemToGrid(ItemWidget, ItemWidgetData.ItemSlotPadding);
}

void UObsidianInventory::OnItemEquipped(const FObsidianItemWidgetData& ItemWidgetData)
{
	if(EquipmentPanel == nullptr)
	{
		return;
	}

	const FGameplayTag DesiredSlot = ItemWidgetData.ItemPosition.GetItemSlotTag();
	const bool bIsForSwapSlot = ItemWidgetData.IsItemForSwapSlot();
	
	checkf(ItemWidgetClass, TEXT("Tried to create widget without valid widget class in UObsidianInventory::OnItemAdded, fill it in ObsidianInventory instance."));
	UObsidianItem* ItemWidget = CreateWidget<UObsidianItem>(this, ItemWidgetClass);
	ItemWidget->InitializeItemWidget(DesiredSlot, ItemWidgetData.GridSpan, ItemWidgetData.ItemImage, bIsForSwapSlot);
	ItemWidget->OnItemLeftMouseButtonPressedDelegate.AddUObject(this, &ThisClass::OnEquipmentItemLeftMouseButtonPressed);
	ItemWidget->OnItemMouseEnterDelegate.AddUObject(this, &ThisClass::OnEquipmentItemMouseEntered);
	ItemWidget->OnItemMouseLeaveDelegate.AddUObject(this, &ThisClass::OnItemMouseLeave);
	
	InventoryItemsWidgetController->RegisterEquipmentItemWidget(DesiredSlot, ItemWidget, ItemWidgetData.bSwappedWithAnotherItem);

	UObsidianItemSlot_Equipment* EquipmentSlot = EquipmentPanel->FindEquipmentSlotForTag(DesiredSlot);
	EquipmentSlot->AddItemToSlot(ItemWidget, ItemWidgetData.ItemSlotPadding);

	if(ItemWidgetData.bDoesBlockSisterSlot)
	{
		const FGameplayTag SisterSlotTag = EquipmentSlot->GetSisterSlotTag();
			
		UObsidianSlotBlockadeItem* BlockedSlotItem = CreateWidget<UObsidianSlotBlockadeItem>(this, SlotBlockadeItemClass);
		BlockedSlotItem->InitializeItemWidget(SisterSlotTag, DesiredSlot, ItemWidgetData.GridSpan, ItemWidgetData.ItemImage, bIsForSwapSlot);
		BlockedSlotItem->OnSlotBlockadeItemLeftMouseButtonPressedDelegate.AddUObject(this, &ThisClass::OnSlotBlockadeItemLeftMouseButtonPressed);
		BlockedSlotItem->OnSlotBlockadeItemMouseEnterDelegate.AddUObject(this, &ThisClass::OnSlotBlockadeItemMouseEntered);
		BlockedSlotItem->OnSlotBlockadeItemMouseLeaveDelegate.AddUObject(this, &ThisClass::OnItemMouseLeave);
		
		InventoryItemsWidgetController->AddBlockedEquipmentItemWidget(DesiredSlot, BlockedSlotItem, false);

		UObsidianItemSlot_Equipment* SlotToBlock = EquipmentPanel->FindEquipmentSlotForTag(SisterSlotTag);
		SlotToBlock->AddItemToSlot(BlockedSlotItem, ItemWidgetData.ItemSlotPadding);
		SlotToBlock->SetSlotState(ISS_Blocked);
		BlockedSlotItem->SetOwningSlot(SlotToBlock);
	}
}

void UObsidianInventory::OnItemChanged(const FObsidianItemWidgetData& ItemWidgetData)
{
	const FIntPoint ItemPosition = ItemWidgetData.ItemPosition.GetItemGridLocation();
	if(UObsidianItem* ItemWidget = InventoryItemsWidgetController->GetItemWidgetAtInventoryGridSlot(ItemPosition))
	{
		ItemWidget->OverrideCurrentStackCount(ItemWidgetData.StackCount);
	}
}

void UObsidianInventory::OnInventoryItemLeftMouseButtonPressed(const UObsidianItem* ItemWidget, const FObsidianItemInteractionFlags& InteractionFlags)
{
	ensureMsgf(ItemWidget, TEXT("Item Widget is invalid in UObsidianInventory::OnInventoryItemLeftMouseButtonPressed"));

	if(InventoryItemsWidgetController)
	{
		if(InteractionFlags.bItemStacksInteraction)
		{
			InventoryItemsWidgetController->HandleLeftClickingOnInventoryItemWithShiftDown(ItemWidget->GetGridPosition(), ItemWidget);
			return;
		}
		InventoryItemsWidgetController->HandleLeftClickingOnInventoryItem(ItemWidget->GetGridPosition(), InteractionFlags.bMoveBetweenNextOpenedWindow);
	}
}

void UObsidianInventory::OnInventoryItemRightMouseButtonPressed(UObsidianItem* ItemWidget)
{
	ensureMsgf(ItemWidget, TEXT("Item Widget is invalid in UObsidianInventory::OnInventoryItemRightMouseButtonPressed"));
	if(InventoryItemsWidgetController)
	{
		InventoryItemsWidgetController->HandleRightClickingOnInventoryItem(ItemWidget->GetGridPosition(), ItemWidget);
	}
}

void UObsidianInventory::OnEquipmentItemLeftMouseButtonPressed(const UObsidianItem* ItemWidget, const FObsidianItemInteractionFlags& InteractionFlags)
{
	ensureMsgf(ItemWidget, TEXT("Item Widget is invalid in UObsidianInventory::OnEquipmentItemLeftMouseButtonPressed"));
	if(InventoryItemsWidgetController)
	{
		InventoryItemsWidgetController->HandleLeftClickingOnEquipmentItem(ItemWidget->GetSlotTag());
	}
}

void UObsidianInventory::OnSlotBlockadeItemLeftMouseButtonPressed(const UObsidianSlotBlockadeItem* SlotBlockadeItem)
{
	ensureMsgf(SlotBlockadeItem, TEXT("Slot Blockade Item Widget is invalid in UObsidianInventory::OnSlotBlockadeItemLeftMouseButtonPressed"));
	if(InventoryItemsWidgetController)
	{
		InventoryItemsWidgetController->HandleLeftClickingOnEquipmentItem(SlotBlockadeItem->GetPrimaryWeaponSlotTag(), SlotBlockadeItem->GetEquipmentSlotTag());
	}
}

void UObsidianInventory::OnSlotBlockadeItemMouseEntered(const UObsidianSlotBlockadeItem* SlotBlockadeItem)
{
	ensureMsgf(SlotBlockadeItem, TEXT("Slot Blockade Item Widget is invalid in UObsidianInventory::OnSlotBlockadeItemMouseEntered"));
	if(InventoryItemsWidgetController)
	{
		InventoryItemsWidgetController->HandleHoveringOverEquipmentItem(InventoryItemsWidgetController->GetItemWidgetAtEquipmentSlot(SlotBlockadeItem->GetPrimaryWeaponSlotTag()));
	}
}

void UObsidianInventory::OnEquipmentItemMouseEntered(const UObsidianItem* ItemWidget)
{
	ensureMsgf(ItemWidget, TEXT("Item Widget is invalid in UObsidianInventory::OnEquipmentItemMouseEntered"));
	if(InventoryItemsWidgetController)
	{
		InventoryItemsWidgetController->HandleHoveringOverEquipmentItem(ItemWidget);
	}
}

void UObsidianInventory::OnInventoryItemMouseEntered(const UObsidianItem* ItemWidget)
{
	ensureMsgf(ItemWidget, TEXT("Item Widget is invalid in UObsidianInventory::OnInventoryItemMouseEntered"));
	if(InventoryItemsWidgetController)
	{
		InventoryItemsWidgetController->HandleHoveringOverInventoryItem(ItemWidget);
	}
}

void UObsidianInventory::OnItemMouseLeave()
{
	if(InventoryItemsWidgetController)
	{
		InventoryItemsWidgetController->HandleUnhoveringItem();
	}
}

void UObsidianInventory::RequestAddingItemToInventory(const FIntPoint& ToPosition, const bool bShiftDown) const
{
	if(InventoryItemsWidgetController)
	{
		InventoryItemsWidgetController->RequestAddingItemToInventory(ToPosition, bShiftDown);
	}
}

void UObsidianInventory::RequestEquippingItem(const FGameplayTag& ToSlot) const
{
	if(InventoryItemsWidgetController)
	{
		InventoryItemsWidgetController->RequestEquippingItem(ToSlot);
	}
}

