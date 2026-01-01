// Copyright 2024 out of sCope team - Michał Ogiński

#include "UI/Inventory/ObsidianInventory.h"

// ~ Core

// ~ Project
#include "UI/Inventory/Slots/ObsidianItemSlot.h"
#include "UI/Inventory/Items/ObsidianItem.h"
#include "UI/Inventory/Slots/ObsidianSlotBlockadeItem.h"
#include "UI/Inventory/Slots/ObsidianItemSlot_Equipment.h"
#include "UI/WidgetControllers/ObInventoryItemsWidgetController.h"
#include "UI/Inventory/ObsidianEquipmentPanel.h"
#include "UI/Inventory/ObsidianGrid.h"

void UObsidianInventory::HandleWidgetControllerSet()
{
	InventoryItemsWidgetController = Cast<UObInventoryItemsWidgetController>(WidgetController);
	check(InventoryItemsWidgetController);

	InventoryItemsWidgetController->OnItemEquippedDelegate.AddUObject(this, &ThisClass::OnItemEquipped);
	InventoryItemsWidgetController->OnEquippedItemRemovedDelegate.AddUObject(this, &ThisClass::OnItemUnequipped);
	
	InventoryItemsWidgetController->OnItemAddedDelegate.AddUObject(this, &ThisClass::OnItemAdded);
	InventoryItemsWidgetController->OnInventoryItemChangedDelegate.AddUObject(this, &ThisClass::OnItemChanged);
	InventoryItemsWidgetController->OnStartPlacementHighlightDelegate.AddUObject(this, &ThisClass::HighlightSlotPlacement);
	InventoryItemsWidgetController->OnStopPlacementHighlightDelegate.AddUObject(this, &ThisClass::StopHighlightSlotPlacement);
}

void UObsidianInventory::NativeConstruct()
{
	Super::NativeConstruct();

	if(InventoryGrid && InventoryItemsWidgetController)
	{
		InventoryGrid->ConstructGrid(InventoryItemsWidgetController, EObsidianGridOwner::Inventory,
			InventoryItemsWidgetController->GetInventoryGridWidth(), InventoryItemsWidgetController->GetInventoryGridHeight());
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
		InventoryItemsWidgetController->OnInventoryItemChangedDelegate.Clear();
	}
	
	if(InventoryGrid)
	{
		InventoryGrid->OnGridSlotPressedDelegate.Clear();
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
	const FIntPoint DesiredPosition = ItemWidgetData.ItemPosition.GetItemGridPosition();
	const FIntPoint GridSpan = ItemWidgetData.GridSpan;
	
	checkf(ItemWidgetClass, TEXT("Tried to create widget without valid widget class in UObsidianInventory::OnItemAdded,"
							  " fill it in ObsidianInventory instance."));
	UObsidianItem* ItemWidget = CreateWidget<UObsidianItem>(this, ItemWidgetClass);
	ItemWidget->InitializeItemWidget(DesiredPosition, GridSpan, ItemWidgetData.ItemImage, ItemWidgetData.StackCount);
	ItemWidget->OnItemLeftMouseButtonPressedDelegate.AddUObject(this, &ThisClass::OnInventoryItemLeftMouseButtonPressed);
	ItemWidget->OnItemMouseEnterDelegate.AddUObject(this, &ThisClass::OnInventoryItemMouseEntered);
	//ItemWidget->OnItemMouseLeaveDelegate.AddUObject(this, &ThisClass::OnItemMouseLeave); //TODO(intrxx) SLOT REFACTOR
	
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
	
	checkf(ItemWidgetClass, TEXT("Tried to create widget without valid widget class in UObsidianInventory::OnItemAdded,"
							  " fill it in ObsidianInventory instance."));
	UObsidianItem* ItemWidget = CreateWidget<UObsidianItem>(this, ItemWidgetClass);
	ItemWidget->InitializeItemWidget(ItemWidgetData.GridSpan, ItemWidgetData.ItemImage, bIsForSwapSlot);
	InventoryItemsWidgetController->RegisterEquipmentItemWidget(DesiredSlot, ItemWidget, ItemWidgetData.bSwappedWithAnotherItem);

	UObsidianItemSlot_Equipment* EquipmentSlot = EquipmentPanel->FindEquipmentSlotWidgetForTag(DesiredSlot);
	EquipmentSlot->AddItemToSlot(ItemWidget, ItemWidgetData.ItemPosition, ItemWidgetData.ItemSlotPadding);
	EquipmentSlot->OnEquippedItemLeftButtonPressedDelegate.AddUObject(this, &ThisClass::OnEquipmentItemLeftMouseButtonPressed);
	EquipmentSlot->OnEquippedItemHoverDelegate.AddUObject(this, &ThisClass::OnEquippedItemHover);

	if(ItemWidgetData.bDoesBlockSisterSlot)
	{
		const FGameplayTag SisterSlotTag = EquipmentSlot->GetSisterSlotTag();
		if (SisterSlotTag.IsValid())
		{
			checkf(SlotBlockadeItemClass, TEXT("Tried to create blocked item widget without valid widget class in"
									  " UObsidianInventory::OnItemAdded, fill it in ObsidianInventory instance."));
			UObsidianSlotBlockadeItem* BlockedSlotItem = CreateWidget<UObsidianSlotBlockadeItem>(this, SlotBlockadeItemClass);
			BlockedSlotItem->InitializeItemWidget(ItemWidgetData.GridSpan,ItemWidgetData.ItemImage, bIsForSwapSlot);
			InventoryItemsWidgetController->AddBlockedEquipmentItemWidget(DesiredSlot, BlockedSlotItem, false);

			UObsidianItemSlot_Equipment* SlotToBlock = EquipmentPanel->FindEquipmentSlotWidgetForTag(SisterSlotTag);
			SlotToBlock->AddBlockadeItemToSlot(BlockedSlotItem, ItemWidgetData.ItemPosition, ItemWidgetData.ItemSlotPadding);
			SlotToBlock->SetSlotState(EObsidianItemSlotState::Blocked, EObsidianItemSlotStatePriority::TakePriority);
			SlotToBlock->OnBlockedSlotLeftButtonPressedDelegate.AddUObject(this, &ThisClass::OnSlotBlockadeItemLeftMouseButtonPressed);
			SlotToBlock->OnBlockedSlotHoverDelegate.AddUObject(this, &ThisClass::OnBlockedSlotItemHover);
		
			BlockedSlotItem->SetOwningSlot(SlotToBlock);
		}
	}
}

void UObsidianInventory::OnItemUnequipped(const FGameplayTag& SlotTag, const bool bBlocksOtherSlot)
{
	UObsidianItemSlot_Equipment* EquipmentSlot = EquipmentPanel->FindEquipmentSlotWidgetForTag(SlotTag);
	EquipmentSlot->Reset();
	
	if (bBlocksOtherSlot)
	{
		UObsidianItemSlot_Equipment* BlockedSlot = EquipmentPanel->FindEquipmentSlotWidgetForTag(
			EquipmentSlot->GetSisterSlotTag());
		BlockedSlot->Reset();
	}
}

void UObsidianInventory::OnItemChanged(const FObsidianItemWidgetData& ItemWidgetData)
{
	const FIntPoint ItemPosition = ItemWidgetData.ItemPosition.GetItemGridPosition();
	if(UObsidianItem* ItemWidget = InventoryItemsWidgetController->GetItemWidgetAtInventoryGridSlot(ItemPosition))
	{
		ItemWidget->OverrideCurrentStackCount(ItemWidgetData.StackCount);
	}
}

void UObsidianInventory::OnInventoryItemLeftMouseButtonPressed(const UObsidianItem* ItemWidget,
	const FObsidianItemInteractionFlags& InteractionFlags)
{
	ensureMsgf(ItemWidget, TEXT("Item Widget is invalid in UObsidianInventory::OnInventoryItemLeftMouseButtonPressed"));

	if(InventoryItemsWidgetController)
	{
		if(InteractionFlags.bItemStacksInteraction)
		{
			InventoryItemsWidgetController->HandleLeftClickingOnInventoryItemWithShiftDown(ItemWidget->GetGridPosition(),
				ItemWidget);
			return;
		}
		InventoryItemsWidgetController->HandleLeftClickingOnInventoryItem(ItemWidget->GetGridPosition(),
			InteractionFlags.bMoveBetweenNextOpenedWindow);
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

void UObsidianInventory::OnEquipmentItemLeftMouseButtonPressed(const UObsidianItemSlot_Equipment* PressedSlot,
	const FObsidianItemPosition& ItemPosition, const FObsidianItemInteractionFlags& InteractionFlags)
{
	if(InventoryItemsWidgetController && PressedSlot)
	{
		InventoryItemsWidgetController->HandleLeftClickingOnEquipmentItem(ItemPosition.GetItemSlotTag());
	}
}

void UObsidianInventory::OnSlotBlockadeItemLeftMouseButtonPressed(const UObsidianItemSlot_Equipment* PressedSlot,
	const FObsidianItemPosition& ItemPosition, const FObsidianItemInteractionFlags& InteractionFlags)
{
	if(InventoryItemsWidgetController && PressedSlot)
	{
		InventoryItemsWidgetController->HandleLeftClickingOnEquipmentItem(ItemPosition.GetItemSlotTag(),
			PressedSlot->GetSlotTag());
	}
}

void UObsidianInventory::OnBlockedSlotItemHover(const FObsidianItemPosition& PrimaryItemPosition, const bool bEntered)
{
	if (InventoryItemsWidgetController == nullptr)
	{
		return;
	}

	if (bEntered)
	{
		const UObsidianItem* HoverOverItemWidget = InventoryItemsWidgetController->GetItemWidgetAtEquipmentSlot(
			PrimaryItemPosition.GetItemSlotTag());
		InventoryItemsWidgetController->HandleHoveringOverEquipmentItem(HoverOverItemWidget, PrimaryItemPosition);
	}
	else
	{
		InventoryItemsWidgetController->HandleUnhoveringItem();
	}
}

void UObsidianInventory::OnEquippedItemHover(const FObsidianItemPosition& ItemPosition, const bool bEntered)
{
	if (InventoryItemsWidgetController == nullptr)
	{
		return;
	}
	
	if (bEntered)
	{
		const UObsidianItem* HoverOverItemWidget = InventoryItemsWidgetController->GetItemWidgetAtEquipmentSlot(
			ItemPosition.GetItemSlotTag());
		InventoryItemsWidgetController->HandleHoveringOverEquipmentItem(HoverOverItemWidget, ItemPosition);
	}
	else
	{
		InventoryItemsWidgetController->HandleUnhoveringItem();
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

void UObsidianInventory::HighlightSlotPlacement(const FGameplayTagContainer& WithTags)
{
	for (UObsidianItemSlot_Equipment* SlotWidget : EquipmentPanel->GetSlotWidgets())
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

