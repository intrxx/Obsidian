// Copyright 2024 out of sCope team - Michał Ogiński

#include "UI/Inventory/ObsidianInventory.h"

// ~ Core

// ~ Project
#include "Obsidian/ObsidianGameModule.h"
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
		InventoryGrid->SetWidgetController(InventoryItemsWidgetController);
		InventoryGrid->ConstructGrid(InventoryItemsWidgetController, EObsidianGridOwner::Inventory,
			InventoryItemsWidgetController->GetInventoryGridWidth(), InventoryItemsWidgetController->GetInventoryGridHeight());
		InventoryGrid->OnGridSlotPressedDelegate.AddUObject(this, &ThisClass::RequestAddingItemToInventory);
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
		UE_LOG(LogObsidian, Error, TEXT("EquipmentPanel is invalid in [%hs]"), __FUNCTION__);
		return;
	}
	
	checkf(ItemWidgetClass, TEXT("Tried to create widget without valid widget class in UObsidianInventory::OnItemAdded,"
							  " fill it in ObsidianInventory instance."));
	UObsidianItem* ItemWidget = CreateWidget<UObsidianItem>(this, ItemWidgetClass);
	ItemWidget->InitializeItemWidget(ItemWidgetData.GridSpan, ItemWidgetData.ItemImage, ItemWidgetData.IsItemForSwapSlot());
	ItemWidget->SetVisibility(ESlateVisibility::HitTestInvisible); //TODO(intrxx) Item Widget Handling Refactor quick hack
	EquipmentPanel->AddItemWidget(ItemWidget, ItemWidgetData);
}

void UObsidianInventory::OnItemUnequipped(const FGameplayTag& SlotTag, const bool bBlocksOtherSlot)
{
	if (EquipmentPanel)
	{
		EquipmentPanel->HandleItemUnequipped(SlotTag, bBlocksOtherSlot);
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

