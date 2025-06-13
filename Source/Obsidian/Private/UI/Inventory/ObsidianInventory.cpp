// Copyright 2024 out of sCope team - Michał Ogiński

#include "UI/Inventory/ObsidianInventory.h"

// ~ Core
#include "Components/SizeBox.h"

// ~ Project
#include "UI/Inventory/SubWidgets/ObsidianItemSlot.h"
#include "UI/Inventory/ObsidianItem.h"
#include "UI/Inventory/ObsidianSlotBlockadeItem.h"
#include "UI/Inventory/SubWidgets/ObsidianItemSlot_Equipment.h"
#include "UI/WidgetControllers/ObsidianInventoryWidgetController.h"
#include "UI/Inventory/ObsidianEquipmentPanel.h"
#include "UI/Inventory/ObsidianInventoryGrid.h"

void UObsidianInventory::HandleWidgetControllerSet()
{
	InventoryWidgetController = Cast<UObsidianInventoryWidgetController>(WidgetController);
	check(InventoryWidgetController);

	InventoryWidgetController->OnItemEquippedDelegate.AddUObject(this, &ThisClass::OnItemEquipped);
	
	InventoryWidgetController->OnItemAddedDelegate.AddUObject(this, &ThisClass::OnItemAdded);
	InventoryWidgetController->OnItemChangedDelegate.AddUObject(this, &ThisClass::OnItemChanged);
}

void UObsidianInventory::NativePreConstruct()
{
	Super::NativePreConstruct();

	Root_SizeBox->SetHeightOverride(RootSizeBoxHeight);
	Root_SizeBox->SetWidthOverride(RootSizeBoxWidth);
}

void UObsidianInventory::NativeConstruct()
{
	Super::NativeConstruct();

	if(InventoryGrid && InventoryWidgetController)
	{
		InventoryGrid->ConstructInventoryGrid(this, InventoryWidgetController->GetInventoryGridWidth(), InventoryWidgetController->GetInventoryGridHeight());
		InventoryGrid->OnInventoryGridSlotPressedDelegate.AddUObject(this, &ThisClass::RequestAddingItemToInventory);
	}
	
	if(EquipmentPanel)
	{
		EquipmentPanel->InitializeEquipmentPanel(this);
		EquipmentPanel->OnEquipmentSlotPressedDelegate.AddUObject(this, &ThisClass::RequestEquippingItem);
	}
}

void UObsidianInventory::NativeDestruct()
{
	if(InventoryWidgetController)
	{
		InventoryWidgetController->RemoveItemUIElements();
	}
	
	if(InventoryGrid)
	{
		InventoryGrid->OnInventoryGridSlotPressedDelegate.Clear();
	}

	if(EquipmentPanel)
	{
		EquipmentPanel->OnEquipmentSlotPressedDelegate.Clear();
	}
	
	Super::NativeDestruct();
}

bool UObsidianInventory::IsPlayerDraggingItem() const
{
	if(InventoryWidgetController == false)
	{
		return false;
	}
	return InventoryWidgetController->IsDraggingAnItem();
}

bool UObsidianInventory::GetDraggedItemGridSize(TArray<FVector2D>& OutItemGridSize) const
{
	if(InventoryWidgetController == false || InventoryWidgetController->IsDraggingAnItem() == false)
	{
		return false;
	}

	InventoryWidgetController->GetDraggedItemGridSize(OutItemGridSize);
	return true;
}

bool UObsidianInventory::CanPlaceDraggedItem(const FVector2D ToHoveredSlotPosition, const TArray<FVector2D>& ItemGridSize) const
{
	if(InventoryWidgetController == false || InventoryWidgetController->IsDraggingAnItem() == false)
	{
		return false;
	}

	return InventoryWidgetController->CanPlaceDraggedItem(ToHoveredSlotPosition, ItemGridSize);
}

bool UObsidianInventory::CanEquipDraggedItem(const FGameplayTag& ToSlotTag) const
{
	if(InventoryWidgetController)
	{
		return InventoryWidgetController->CanEquipDraggedItem(ToSlotTag);
	}
	return false;
}

void UObsidianInventory::OnItemAdded(const FObsidianItemWidgetData& ItemWidgetData)
{
	const FVector2D DesiredPosition = ItemWidgetData.DesiredPosition;
	const FVector2D GridSpan = ItemWidgetData.GridSpan;
	
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
	
	InventoryWidgetController->RegisterInventoryItemWidget(DesiredPosition, ItemWidget);
	InventoryGrid->AddItemToGrid(ItemWidget, ItemWidgetData.ItemSlotPadding);
}

void UObsidianInventory::OnItemEquipped(const FObsidianItemWidgetData& ItemWidgetData)
{
	if(EquipmentPanel == nullptr)
	{
		return;
	}
	
	const FGameplayTag DesiredSlot = ItemWidgetData.DesiredSlot;
	
	checkf(ItemWidgetClass, TEXT("Tried to create widget without valid widget class in UObsidianInventory::OnItemAdded, fill it in ObsidianInventory instance."));
	UObsidianItem* ItemWidget = CreateWidget<UObsidianItem>(this, ItemWidgetClass);
	ItemWidget->InitializeItemWidget(DesiredSlot, ItemWidgetData.GridSpan, ItemWidgetData.ItemImage);
	ItemWidget->OnItemLeftMouseButtonPressedDelegate.AddUObject(this, &ThisClass::OnEquipmentItemLeftMouseButtonPressed);
	ItemWidget->OnItemMouseEnterDelegate.AddUObject(this, &ThisClass::OnEquipmentItemMouseEntered);
	ItemWidget->OnItemMouseLeaveDelegate.AddUObject(this, &ThisClass::OnItemMouseLeave);
	InventoryWidgetController->RegisterEquipmentItemWidget(DesiredSlot, ItemWidget, ItemWidgetData.bSwappedWithAnotherItem);

	UObsidianItemSlot_Equipment* EquipmentSlot = EquipmentPanel->FindEquipmentSlotForTag(DesiredSlot);
	EquipmentSlot->AddItemToSlot(ItemWidget, ItemWidgetData.ItemSlotPadding);

	if(ItemWidgetData.bDoesBlockSisterSlot)
	{
		const FGameplayTag SisterSlotTag = EquipmentSlot->GetSisterSlotTag();
			
		UObsidianSlotBlockadeItem* BlockedSlotItem = CreateWidget<UObsidianSlotBlockadeItem>(this, SlotBlockadeItemClass);
		BlockedSlotItem->InitializeItemWidget(SisterSlotTag, DesiredSlot, ItemWidgetData.GridSpan, ItemWidgetData.ItemImage);
		BlockedSlotItem->OnSlotBlockadeItemLeftMouseButtonPressedDelegate.AddUObject(this, &ThisClass::OnSlotBlockadeItemLeftMouseButtonPressed);
		BlockedSlotItem->OnSlotBlockadeItemMouseEnterDelegate.AddUObject(this, &ThisClass::OnSlotBlockadeItemMouseEntered);
		BlockedSlotItem->OnSlotBlockadeItemMouseLeaveDelegate.AddUObject(this, &ThisClass::OnItemMouseLeave);
		InventoryWidgetController->AddBlockedEquipmentItemWidget(DesiredSlot, BlockedSlotItem, false);

		UObsidianItemSlot_Equipment* SlotToBlock = EquipmentPanel->FindEquipmentSlotForTag(SisterSlotTag);
		SlotToBlock->AddItemToSlot(BlockedSlotItem, ItemWidgetData.ItemSlotPadding);
		SlotToBlock->SetSlotState(ISS_Blocked);
		BlockedSlotItem->SetOwningSlot(SlotToBlock);
	}
}

void UObsidianInventory::OnItemChanged(const FObsidianItemWidgetData& ItemWidgetData)
{
	const FVector2D ItemPosition = ItemWidgetData.DesiredPosition;
	if(UObsidianItem* ItemWidget = InventoryWidgetController->GetItemWidgetAtInventoryLocation(ItemPosition))
	{
		ItemWidget->OverrideCurrentStackCount(ItemWidgetData.StackCount);
	}
}

void UObsidianInventory::OnInventoryItemLeftMouseButtonPressed(const UObsidianItem* ItemWidget, const bool bShiftDown)
{
	ensureMsgf(ItemWidget, TEXT("Item Widget is invalid in UObsidianInventory::OnInventoryItemLeftMouseButtonPressed"));

	if(InventoryWidgetController)
	{
		if(bShiftDown)
		{
			InventoryWidgetController->HandleLeftClickingOnInventoryItemWithShiftDown(ItemWidget->GetInventoryPosition(), ItemWidget);
			return;
		}
		InventoryWidgetController->HandleLeftClickingOnInventoryItem(ItemWidget->GetInventoryPosition());
	}
}

void UObsidianInventory::OnInventoryItemRightMouseButtonPressed(UObsidianItem* ItemWidget)
{
	ensureMsgf(ItemWidget, TEXT("Item Widget is invalid in UObsidianInventory::OnInventoryItemRightMouseButtonPressed"));
	if(InventoryWidgetController)
	{
		InventoryWidgetController->HandleRightClickingOnInventoryItem(ItemWidget->GetInventoryPosition(), ItemWidget);
	}
}

void UObsidianInventory::OnEquipmentItemLeftMouseButtonPressed(const UObsidianItem* ItemWidget, const bool bShiftDown)
{
	ensureMsgf(ItemWidget, TEXT("Item Widget is invalid in UObsidianInventory::OnEquipmentItemLeftMouseButtonPressed"));
	if(InventoryWidgetController)
	{
		InventoryWidgetController->HandleLeftClickingOnEquipmentItem(ItemWidget->GetEquipmentSlotTag());
	}
}

void UObsidianInventory::OnSlotBlockadeItemLeftMouseButtonPressed(const UObsidianSlotBlockadeItem* SlotBlockadeItem)
{
	ensureMsgf(SlotBlockadeItem, TEXT("Slot Blockade Item Widget is invalid in UObsidianInventory::OnSlotBlockadeItemLeftMouseButtonPressed"));
	if(InventoryWidgetController)
	{
		InventoryWidgetController->HandleLeftClickingOnEquipmentItem(SlotBlockadeItem->GetPrimaryWeaponSlotTag(), SlotBlockadeItem->GetEquipmentSlotTag());
	}
}

void UObsidianInventory::OnSlotBlockadeItemMouseEntered(const UObsidianSlotBlockadeItem* SlotBlockadeItem)
{
	ensureMsgf(SlotBlockadeItem, TEXT("Slot Blockade Item Widget is invalid in UObsidianInventory::OnSlotBlockadeItemMouseEntered"));
	if(InventoryWidgetController)
	{
		InventoryWidgetController->HandleHoveringOverEquipmentItem(InventoryWidgetController->GetItemWidgetAtEquipmentSlot(SlotBlockadeItem->GetPrimaryWeaponSlotTag()));
	}
}

void UObsidianInventory::OnEquipmentItemMouseEntered(const UObsidianItem* ItemWidget)
{
	ensureMsgf(ItemWidget, TEXT("Item Widget is invalid in UObsidianInventory::OnEquipmentItemMouseEntered"));
	if(InventoryWidgetController)
	{
		InventoryWidgetController->HandleHoveringOverEquipmentItem(ItemWidget);
	}
}

void UObsidianInventory::OnInventoryItemMouseEntered(const UObsidianItem* ItemWidget)
{
	ensureMsgf(ItemWidget, TEXT("Item Widget is invalid in UObsidianInventory::OnInventoryItemMouseEntered"));
	if(InventoryWidgetController)
	{
		InventoryWidgetController->HandleHoveringOverInventoryItem(ItemWidget);
	}
}

void UObsidianInventory::OnItemMouseLeave()
{
	InventoryWidgetController->HandleUnhoveringItem();
}

void UObsidianInventory::RequestAddingItemToInventory(const FVector2D& ToPosition, const bool bShiftDown) const
{
	if(InventoryWidgetController)
	{
		InventoryWidgetController->RequestAddingItemToInventory(ToPosition, bShiftDown);
	}
}

void UObsidianInventory::RequestEquippingItem(const FGameplayTag& ToSlot) const
{
	if(InventoryWidgetController)
	{
		InventoryWidgetController->RequestEquippingItem(ToSlot);
	}
}

