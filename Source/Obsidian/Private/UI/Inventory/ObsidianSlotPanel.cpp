// Copyright 2024 out of sCope team - Michał Ogiński

#include "UI/Inventory/ObsidianSlotPanel.h"

#include <Blueprint/WidgetTree.h>

#include "Obsidian/ObsidianGameModule.h"
#include "Obsidian/Public/UI/Inventory/Slots/ObsidianItemSlot_Equipment.h"
#include "UI/Inventory/Items/ObsidianItem.h"
#include "UI/Inventory/Slots/ObsidianSlotBlockadeItem.h"
#include "UI/WidgetControllers/ObInventoryItemsWidgetController.h"

void UObsidianSlotPanel::HandleWidgetControllerSet()
{
	InventoryItemsWidgetController = Cast<UObInventoryItemsWidgetController>(WidgetController);
	check(InventoryItemsWidgetController);
	
	InitializeEquipmentPanel();
}

void UObsidianSlotPanel::InitializeEquipmentPanel()
{
	WidgetTree->ForEachWidget([this](UWidget* Widget)
		{
			if(UObsidianItemSlot_Equipment* EquipmentSlot = Cast<UObsidianItemSlot_Equipment>(Widget))
			{
				EquipmentSlot->OnEquipmentSlotHoverDelegate.AddUObject(this, &ThisClass::OnEquipmentSlotHover);
				EquipmentSlot->OnEquipmentSlotPressedDelegate.AddUObject(this, &ThisClass::OnEquipmentSlotMouseButtonDown);
				EquipmentSlots.Add(EquipmentSlot);
			}
		});
}

void UObsidianSlotPanel::NativeDestruct()
{
	for (UObsidianItemSlot_Equipment* EquipmentSlot : EquipmentSlots)
	{
		if (EquipmentSlot)
		{
			EquipmentSlot->OnEquipmentSlotHoverDelegate.Clear();
			EquipmentSlot->OnEquipmentSlotPressedDelegate.Clear();
		}
	}
	
	Super::NativeDestruct();
}

UObsidianItemSlot_Equipment* UObsidianSlotPanel::FindEquipmentSlotWidgetForTag(const FGameplayTag& Tag) const
{
	for(UObsidianItemSlot_Equipment* EquipmentSlot : EquipmentSlots)
	{
		if(EquipmentSlot->GetSlotTag() == Tag)
		{
			return EquipmentSlot;
		}
	}
	return nullptr;
}

TArray<UObsidianItemSlot_Equipment*> UObsidianSlotPanel::GetSlotWidgets() const
{
	return EquipmentSlots;
}

bool UObsidianSlotPanel::IsEquipmentSlotOccupiedByItem(const FGameplayTag& AtSlot) const
{
	return EquippedItemWidgetMap.Contains(AtSlot);
}

UObsidianItem* UObsidianSlotPanel::GetItemWidgetAtEquipmentSlot(const FGameplayTag& AtSlot) const
{
	if(UObsidianItem* const* ItemWidgetValuePtr = EquippedItemWidgetMap.Find(AtSlot))
	{
		return *ItemWidgetValuePtr;
	}
	return nullptr;
}

bool UObsidianSlotPanel::IsEquipmentSlotOccupiedByBlockade(const FGameplayTag& AtSlot) const
{
	return BlockedSlotsWidgetMap.Contains(AtSlot);
}

UObsidianSlotBlockadeItem* UObsidianSlotPanel::GetBlockadeItemWidgetAtEquipmentSlot(const FGameplayTag& AtSlot) const
{
	if(UObsidianSlotBlockadeItem* const* ItemBlockadeWidgetValuePtr = BlockedSlotsWidgetMap.Find(AtSlot))
	{
		return *ItemBlockadeWidgetValuePtr;
	}
	return nullptr;
}

void UObsidianSlotPanel::AddItemWidget(UObsidianItem* ItemWidget, const FObsidianItemWidgetData& ItemWidgetData)
{
	if (ItemWidget == nullptr)
	{
		UE_LOG(LogObsidian, Error, TEXT("ItemWidget to Add Item Widget to Equipment Panel is invalid in [%hs]"),
			__FUNCTION__);
		return;
	}
	
	const FGameplayTag DesiredSlotTag = ItemWidgetData.ItemPosition.GetItemSlotTag();
	if (DesiredSlotTag.IsValid() == false)
	{
		UE_LOG(LogObsidian, Error, TEXT("Desired Slot to Add Item Widget to Equipment Panel is invalid in [%hs]"),
			__FUNCTION__);
		return;
	}

	UObsidianItemSlot_Equipment* EquipmentSlot = FindEquipmentSlotWidgetForTag(DesiredSlotTag);
	if (EquipmentSlot == nullptr)
	{
		UE_LOG(LogObsidian, Error, TEXT("Unable to find Equipment Slot for tag [%s] on Equipment Panel in [%hs]"),
			*DesiredSlotTag.ToString(), __FUNCTION__);
		return;
	}

	EquipmentSlot->AddItemToSlot(ItemWidget, ItemWidgetData.ItemSlotPadding);
	RegisterEquipmentItemWidget(DesiredSlotTag, ItemWidget, ItemWidgetData.bSwappedWithAnotherItem);

	if(ItemWidgetData.bDoesBlockSisterSlot) // Item blocks sister slot, add blockade item
	{
		const FGameplayTag SisterSlotTag = EquipmentSlot->GetSisterSlotTag();
		if (SisterSlotTag.IsValid() == false)
		{
			UE_LOG(LogObsidian, Error, TEXT("Equipment Slot with Tag [%s] has no Sister Slot but the Item added"
								   " to it want to block it [%hs], please verify if the item and slots are set up correctly."),
				*EquipmentSlot->GetSlotTag().ToString(), __FUNCTION__);
			return;
		}

		UObsidianItemSlot_Equipment* SlotToBlock = FindEquipmentSlotWidgetForTag(SisterSlotTag);
		if (SlotToBlock == nullptr)
		{
			UE_LOG(LogObsidian, Error, TEXT("Unable to find Slot To Block for tag [%s] on Equipment Panel in [%hs]"),
				*SisterSlotTag.ToString(), __FUNCTION__);
			return;
		}

		checkf(SlotBlockadeItemClass, TEXT("Tried to create blocked item widget without valid widget class in"
								  " UObsidianInventory::OnItemAdded, fill it in ObsidianInventory instance."));
		UObsidianSlotBlockadeItem* BlockedSlotItem = CreateWidget<UObsidianSlotBlockadeItem>(this,
			SlotBlockadeItemClass);
		BlockedSlotItem->InitializeItemWidget(ItemWidgetData.GridSpan,ItemWidgetData.ItemImage,
			ItemWidgetData.IsItemForSwapSlot());
		BlockedSlotItem->SetVisibility(ESlateVisibility::HitTestInvisible); //TODO(intrxx) Item Widget Handling Refactor quick hack
		BlockedSlotItem->SetOwningSlot(SlotToBlock);
		
		SlotToBlock->AddBlockadeItemToSlot(BlockedSlotItem, ItemWidgetData.ItemSlotPadding);
		SlotToBlock->SetSlotState(EObsidianItemSlotState::Blocked, EObsidianItemSlotStatePriority::TakePriority);
		RegisterBlockedEquipmentItemWidget(SisterSlotTag, BlockedSlotItem, false);
	}
}

void UObsidianSlotPanel::OnEquipmentSlotHover(UObsidianItemSlot_Equipment* AffectedSlot, const bool bEntered)
{
	if(InventoryItemsWidgetController == nullptr)
	{
		UE_LOG(LogObsidian, Error, TEXT("InventoryItemsWidgetController is invalid in [%hs]."), __FUNCTION__)
		return;
	}

	if (AffectedSlot == nullptr)
	{
		UE_LOG(LogObsidian, Error, TEXT("AffectedSlot is invalid in [%hs]."), __FUNCTION__)
		return;
	}
	
	const FGameplayTag SlotTag = AffectedSlot->GetSlotTag();
	check(SlotTag.IsValid());

	const bool bCanInteract = InventoryItemsWidgetController->CanInteractWithEquipment();
	const bool IsSlotOccupied = IsEquipmentSlotOccupiedByItem(SlotTag);
	const bool IsSlotBlocked = IsEquipmentSlotOccupiedByBlockade(SlotTag);
	const bool IsSlotEmpty = IsSlotBlocked == false && IsSlotOccupied == false;
	const FGameplayTag ItemHoverPosition = IsSlotBlocked ? AffectedSlot->GetSisterSlotTag() :
											IsSlotOccupied ? AffectedSlot->GetSlotTag() :
											 FGameplayTag::EmptyTag;
	
	if (bEntered)
	{
		if (IsSlotEmpty == false)
		{
			if (const UObsidianItem* HoverOverItemWidget = GetItemWidgetAtEquipmentSlot(ItemHoverPosition))
			{
				InventoryItemsWidgetController->HandleHoveringOverItem(ItemHoverPosition, HoverOverItemWidget);	
			}
		}
		
		if(bCanInteract == false)
		{
			AffectedSlot->SetSlotState(EObsidianItemSlotState::RedLight, EObsidianItemSlotStatePriority::Low);
			return;
		}
	
		if(InventoryItemsWidgetController->IsDraggingAnItem() == false)
		{
			AffectedSlot->SetSlotState(EObsidianItemSlotState::Selected, EObsidianItemSlotStatePriority::Low);
			return;
		}
		
		const bool bInteractionSuccess = InventoryItemsWidgetController->CanEquipDraggedItem(AffectedSlot->GetSlotTag());
		const EObsidianItemSlotState SlotState = bInteractionSuccess ? EObsidianItemSlotState::GreenLight : EObsidianItemSlotState::RedLight;
		AffectedSlot->SetSlotState(SlotState, EObsidianItemSlotStatePriority::Low);
	}
	else
	{
		if (IsSlotEmpty == false)
		{
			InventoryItemsWidgetController->HandleUnhoveringItem(ItemHoverPosition);
		}

		if (bCanInteract)
		{
			AffectedSlot->SetSlotState(EObsidianItemSlotState::Neutral, EObsidianItemSlotStatePriority::Low);
		}
	}
}

void UObsidianSlotPanel::OnEquipmentSlotMouseButtonDown(const UObsidianItemSlot_Equipment* AffectedSlot,
	const FObsidianItemInteractionFlags& InteractionFlags) const
{
	if(InventoryItemsWidgetController == nullptr)
	{
		UE_LOG(LogObsidian, Error, TEXT("InventoryItemsWidgetController is invalid in [%hs]."), __FUNCTION__)
		return;
	}

	if (AffectedSlot == nullptr)
	{
		UE_LOG(LogObsidian, Error, TEXT("AffectedSlot is invalid in [%hs]."), __FUNCTION__)
		return;
	}
	
	const FGameplayTag SlotTag = AffectedSlot->GetSlotTag();
	check(SlotTag.IsValid());

	if (IsEquipmentSlotOccupiedByItem(SlotTag))
	{
		InventoryItemsWidgetController->HandleLeftClickingOnEquipmentItem(SlotTag);
	}
	else if (IsEquipmentSlotOccupiedByBlockade(SlotTag))
	{
		InventoryItemsWidgetController->HandleLeftClickingOnEquipmentItem(AffectedSlot->GetSisterSlotTag(),
			SlotTag);
	}
	else
	{
		InventoryItemsWidgetController->RequestEquippingItem(SlotTag);
	}
}

void UObsidianSlotPanel::HandleItemUnequipped(const FGameplayTag& SlotToClearTag, const bool bBlocksOtherSlot)
{
	if (const UObsidianItemSlot_Equipment* SlotToClear = FindEquipmentSlotWidgetForTag(SlotToClearTag))
	{
		RemoveEquipmentItemWidget(SlotToClearTag);
		
		if (bBlocksOtherSlot)
		{
			RemoveBlockedSlotItemWidget(SlotToClear->GetSisterSlotTag());
		}
	}
}

void UObsidianSlotPanel::RegisterEquipmentItemWidget(const FGameplayTag& AtSlot, UObsidianItem* ItemWidget,
                                                          const bool bSwappedWithAnother)
{
	if (ensureMsgf(ItemWidget && AtSlot.IsValid(), TEXT("ItemWidget or AtSlot are invalid in [%hs]. "),
		__FUNCTION__))
	{
		if(bSwappedWithAnother)
		{
			RemoveEquipmentItemWidget(AtSlot);
		}
	
		if(!EquippedItemWidgetMap.Contains(AtSlot))
		{
			EquippedItemWidgetMap.Add(AtSlot, ItemWidget);
		}
		else
		{
			UE_LOG(LogObsidian, Error, TEXT("Trying to register ItemWidget [%s] at slot [%s], but this slot is"
								   " already registered. Make sure the logic is right."),
								   *GetNameSafe(ItemWidget), *AtSlot.ToString());
		}
	}
}

void UObsidianSlotPanel::RemoveEquipmentItemWidget(const FGameplayTag& AtSlot)
{
	if  (ensureMsgf(AtSlot.IsValid(), TEXT("AtSlot is invalid in [%hs]. "), __FUNCTION__))
	{
		UObsidianItem* RemovedItemWidget = nullptr;
		if (EquippedItemWidgetMap.RemoveAndCopyValue(AtSlot, RemovedItemWidget))
		{
			if (RemovedItemWidget == nullptr)
			{
				return;
			}
		
			if (ensure(InventoryItemsWidgetController))
			{
				InventoryItemsWidgetController->ClearItemDescriptionForPosition(AtSlot);
			}
			RemovedItemWidget->RemoveFromParent();
		}
		else
		{
			UE_LOG(LogObsidian, Warning, TEXT("Trying to remove ItemWidget from Equipment Slot at [%s],"
									 " but the ItemWidget no longer exists!"), *AtSlot.ToString());
		}
	}
}

void UObsidianSlotPanel::RegisterBlockedEquipmentItemWidget(const FGameplayTag& AtSlot,
	UObsidianSlotBlockadeItem* ItemWidget, const bool bSwappedWithAnother)
{
	if (ensureMsgf(ItemWidget && AtSlot.IsValid(), TEXT("ItemWidget or AtSlot are invalid in [%hs]. "),
		__FUNCTION__))
	{
		if(bSwappedWithAnother)
		{
			RemoveBlockedSlotItemWidget(AtSlot);
		}
	
		if(!BlockedSlotsWidgetMap.Contains(AtSlot))
		{
			BlockedSlotsWidgetMap.Add(AtSlot, ItemWidget);
		}
		else
		{
			UE_LOG(LogObsidian, Error, TEXT("Trying to register ItemWidget [%s] at slot [%s], but this slot is"
								   " already registered. Make sure the logic is right."),
								   *GetNameSafe(ItemWidget), *AtSlot.ToString());
		}
	}
}

void UObsidianSlotPanel::RemoveBlockedSlotItemWidget(const FGameplayTag& AtSlot)
{
	UObsidianSlotBlockadeItem* RemovedItemWidget = nullptr;
	if (BlockedSlotsWidgetMap.RemoveAndCopyValue(AtSlot, RemovedItemWidget))
	{
		if (RemovedItemWidget == nullptr)
		{
			return;
		}
		
		RemovedItemWidget->RemoveFromParent();
	}
}

