// Copyright 2024 out of sCope team - Michał Ogiński

#include "UI/Inventory/ObsidianSlotPanel.h"

#include <Blueprint/WidgetTree.h>

#include "Obsidian/ObsidianGameModule.h"
#include "Obsidian/Public/UI/Inventory/Slots/ObsidianItemSlot_Equipment.h"
#include "UI/Inventory/Items/ObsidianItem.h"
#include "UI/WidgetControllers/ObInventoryItemsWidgetController.h"

// ~ Start of FObsidianSlotData

bool FObsidianSlotData::IsOccupied() const
{
	return bOccupied;
}

void FObsidianSlotData::AddNewItem(const FObsidianItemPosition& InPosition, UObsidianItem* InItemWidget,
	const bool bBlockSlot)
{
	OriginPosition = InPosition;
	ItemWidget = InItemWidget;
	bBlocked = bBlockSlot;
	bOccupied = true;
}

void FObsidianSlotData::Reset()
{
	OriginPosition.Reset();
	ItemWidget = nullptr;
	bBlocked = false;
	bOccupied = false;
}

// ~ End of FObsidianSlotData

void UObsidianSlotPanel::HandleWidgetControllerSet()
{
	InventoryItemsWidgetController = Cast<UObInventoryItemsWidgetController>(WidgetController);
	check(InventoryItemsWidgetController);
	
	InitializeEquipmentPanel();
}

TArray<UObsidianItemSlot_Equipment*> UObsidianSlotPanel::GetAllSlots() const
{
	TArray<UObsidianItemSlot_Equipment*> Slots;
	Slots.Reserve(SlotDataMap.Num());
	
	for (const TPair<FGameplayTag, FObsidianSlotData>& SlotDataPair : SlotDataMap)
	{
		if (UObsidianItemSlot_Equipment* EquipmentSlot = SlotDataPair.Value.OwningSlot)
		{
			Slots.Add(EquipmentSlot);
		}
	}

	return Slots;
}

void UObsidianSlotPanel::InitializeEquipmentPanel()
{
	WidgetTree->ForEachWidget([this](UWidget* Widget)
		{
			if(UObsidianItemSlot_Equipment* EquipmentSlot = Cast<UObsidianItemSlot_Equipment>(Widget))
			{
				EquipmentSlot->OnEquipmentSlotHoverDelegate.AddUObject(this, &ThisClass::OnEquipmentSlotHover);
				EquipmentSlot->OnEquipmentSlotPressedDelegate.AddUObject(this, &ThisClass::OnEquipmentSlotMouseButtonDown);
				
				FObsidianSlotData NewData;
				NewData.OwningSlot = EquipmentSlot;
				SlotDataMap.Add(EquipmentSlot->GetSlotTag(), NewData);
			}
		});
}

void UObsidianSlotPanel::NativeDestruct()
{
	for (const TPair<FGameplayTag, FObsidianSlotData>& SlotDataPair : SlotDataMap)
	{
		if (UObsidianItemSlot_Equipment* EquipmentSlot = SlotDataPair.Value.OwningSlot)
		{
			EquipmentSlot->OnEquipmentSlotHoverDelegate.Clear();
			EquipmentSlot->OnEquipmentSlotPressedDelegate.Clear();
		}
	}
	
	Super::NativeDestruct();
}


UObsidianItemSlot_Equipment* UObsidianSlotPanel::GetSlotByPosition(const FGameplayTag& AtSlotTag)
{
	if (const FObsidianSlotData* SlotData = SlotDataMap.Find(AtSlotTag))
	{
		return SlotData->OwningSlot;
	}
	return nullptr;
}

const FObsidianSlotData* UObsidianSlotPanel::GetSlotDataAtGridPosition(const FGameplayTag& AtSlotTag) const
{
	return SlotDataMap.Find(AtSlotTag);
}

UObsidianItem* UObsidianSlotPanel::GetItemWidgetAtSlot(const FGameplayTag& AtSlotTag) const
{
	const FObsidianSlotData* SlotData = SlotDataMap.Find(AtSlotTag);
	if (SlotData && SlotData->IsOccupied())
	{
		return SlotData->ItemWidget;
	}
	return nullptr;
}

bool UObsidianSlotPanel::IsSlotOccupied(const FGameplayTag& AtSlotTag) const
{
	if (const FObsidianSlotData* SlotData = SlotDataMap.Find(AtSlotTag))
	{
		return SlotData->IsOccupied();
	}
	return false;
}

bool UObsidianSlotPanel::IsSlotBlocked(const FGameplayTag& AtSlotTag) const
{
	if (const FObsidianSlotData* SlotData = SlotDataMap.Find(AtSlotTag))
	{
		return SlotData->bBlocked;
	}
	return false;
}

void UObsidianSlotPanel::AddItemWidget(UObsidianItem* ItemWidget, const FObsidianItemWidgetData& ItemWidgetData,
	const bool bBlockSlot)
{
	if (ItemWidget == nullptr)
	{
		UE_LOG(LogObsidian, Error, TEXT("ItemWidget to Add Item Widget to Equipment Panel is invalid in [%hs]"),
			__FUNCTION__);
		return;
	}
	
	const FGameplayTag SlotTag = ItemWidgetData.ItemPosition.GetItemSlotTag();
	if (SlotTag.IsValid() == false)
	{
		UE_LOG(LogObsidian, Error, TEXT("Slot Tag to Add Item Widget to Equipment Panel is invalid in [%hs]"),
			__FUNCTION__);
		return;
	}

	UObsidianItemSlot_Equipment* EquipmentSlot = GetSlotByPosition(SlotTag);
	if (EquipmentSlot == nullptr)
	{
		UE_LOG(LogObsidian, Error, TEXT("Unable to find Equipment Slot for tag [%s] on Equipment Panel in [%hs]"),
			*SlotTag.ToString(), __FUNCTION__);
		return;
	}

	if (bBlockSlot == false)
	{
		EquipmentSlot->AddItemToSlot(ItemWidget, ItemWidgetData.ItemSlotPadding);
		RegisterSlotItemWidget(ItemWidgetData.ItemPosition, ItemWidget, ItemWidgetData.bSwappedWithAnotherItem);
	}
	else
	{
		const FGameplayTag SisterSlotTag = EquipmentSlot->GetSisterSlotTag();
		if (SisterSlotTag.IsValid() == false)
		{
			UE_LOG(LogObsidian, Error, TEXT("Equipment Slot with Tag [%s] has no Sister Slot but the Item added"
								   " to it want to block it [%hs], please verify if the item and slots are set up correctly."),
				*EquipmentSlot->GetSlotTag().ToString(), __FUNCTION__);
			return;
		}

		UObsidianItemSlot_Equipment* SlotToBlock = GetSlotByPosition(SisterSlotTag);
		if (SlotToBlock == nullptr)
		{
			UE_LOG(LogObsidian, Error, TEXT("Unable to find Slot To Block for tag [%s] on Equipment Panel in [%hs]"),
				*SisterSlotTag.ToString(), __FUNCTION__);
			return;
		}

		SlotToBlock->AddBlockadeItemToSlot(ItemWidget, ItemWidgetData.ItemSlotPadding);
		SlotToBlock->SetSlotState(EObsidianItemSlotState::Blocked, EObsidianItemSlotStatePriority::TakePriority);
		RegisterSlotItemWidget(SisterSlotTag, ItemWidget, false, true,
			ItemWidgetData.ItemPosition);
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

	const FObsidianSlotData* SlotData = GetSlotDataAtGridPosition(SlotTag);
	if (SlotData == nullptr)
	{
		UE_LOG(LogObsidian, Error, TEXT("Could not find SlotData for tag [%s] in [%hs]."),
			*SlotTag.ToString(), __FUNCTION__)
		return;
	}
	
	const bool bCanInteract = InventoryItemsWidgetController->CanInteractWithEquipment();
	const bool IsSlotOccupied = SlotData->IsOccupied();
	const bool IsSlotBlocked = SlotData->bBlocked;
	const bool IsSlotEmpty = IsSlotBlocked == false && IsSlotOccupied == false;
	
	if (bEntered)
	{
		if (IsSlotEmpty == false)
		{
			if (const UObsidianItem* HoverOverItemWidget = SlotData->ItemWidget)
			{
				InventoryItemsWidgetController->HandleHoveringOverItem(SlotData->OriginPosition, HoverOverItemWidget);	
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
		const EObsidianItemSlotState SlotState = bInteractionSuccess ? EObsidianItemSlotState::GreenLight
			: EObsidianItemSlotState::RedLight;
		AffectedSlot->SetSlotState(SlotState, EObsidianItemSlotStatePriority::Low);
	}
	else
	{
		if (IsSlotEmpty == false)
		{
			InventoryItemsWidgetController->HandleUnhoveringItem(SlotData->OriginPosition);
		}

		if (bCanInteract)
		{
			AffectedSlot->SetSlotState(EObsidianItemSlotState::Neutral, EObsidianItemSlotStatePriority::Low);
		}
	}
}

void UObsidianSlotPanel::OnEquipmentSlotMouseButtonDown(const UObsidianItemSlot_Equipment* AffectedSlot,
	const FObsidianItemInteractionFlags& InteractionFlags)
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

	const FObsidianSlotData* SlotData = GetSlotDataAtGridPosition(SlotTag);
	if (SlotData == nullptr)
	{
		UE_LOG(LogObsidian, Error, TEXT("Could not find SlotData for tag [%s] in [%hs]."),
			*SlotTag.ToString(), __FUNCTION__)
		return;
	}
	
	if (SlotData->bBlocked)
	{
		InventoryItemsWidgetController->HandleLeftClickingOnEquipmentItem(SlotTag,
			SlotData->OriginPosition.GetItemSlotTag());
	}
	else if (SlotData->IsOccupied())
	{
		InventoryItemsWidgetController->HandleLeftClickingOnEquipmentItem(SlotTag);
	}
	else
	{
		InventoryItemsWidgetController->RequestEquippingItem(SlotTag);
	}
}

void UObsidianSlotPanel::HandleItemUnequipped(const FGameplayTag& SlotToClearTag, const bool bBlocksOtherSlot)
{
	if (ensureMsgf(SlotToClearTag.IsValid(), TEXT("SlotToClearTag is invalid in [%hs]. "), __FUNCTION__))
	{
		UnregisterSlotItemWidget(SlotToClearTag);

		if (bBlocksOtherSlot)
		{
			if (const FObsidianSlotData* SlotData = SlotDataMap.Find(SlotToClearTag))
			{
				if (const UObsidianItemSlot_Equipment* OwningSlot = SlotData->OwningSlot)
				{
					const FGameplayTag NextSlotTagToClear = OwningSlot->GetSisterSlotTag();
					if (UObsidianItemSlot_Equipment* SisterSlot = GetSlotByPosition(NextSlotTagToClear))
					{
						UnregisterSlotItemWidget(NextSlotTagToClear);
						SisterSlot->ResetSlotState();
					}
				}
			}
		}
	}
	
}

void UObsidianSlotPanel::RegisterSlotItemWidget(const FObsidianItemPosition& ItemPosition, UObsidianItem* ItemWidget,
	const bool bSwappedWithAnother, const bool bBlocksSlot, const FObsidianItemPosition& ItemOriginPosition)
{
	if (ensureMsgf(ItemWidget && ItemPosition.IsValid(), TEXT("ItemWidget or ItemPosition are invalid in [%hs]. "),
		__FUNCTION__))
	{
		if(bSwappedWithAnother)
		{
			UnregisterSlotItemWidget(ItemPosition.GetItemSlotTag());
		}
		
		if (FObsidianSlotData* SlotData = SlotDataMap.Find(ItemPosition.GetItemSlotTag()))
		{
			check(SlotData->IsOccupied() == false);
			const FObsidianItemPosition OriginPosition = bBlocksSlot ? ItemOriginPosition : ItemPosition;
			SlotData->AddNewItem(OriginPosition, ItemWidget, bBlocksSlot);
		}
	}
}

void UObsidianSlotPanel::UnregisterSlotItemWidget(const FGameplayTag& SlotTag)
{
	if  (ensureMsgf(SlotTag.IsValid(), TEXT("SlotTag is invalid in [%hs]. "), __FUNCTION__))
	{
		if (FObsidianSlotData* SlotData = SlotDataMap.Find(SlotTag))
		{
			check(SlotData->IsOccupied());

			if (SlotData->ItemWidget == nullptr)
			{
				UE_LOG(LogTemp, Error, TEXT("Trying to remove ItemWidget from [%s], but the ItemWidget is invalid!"),
					*SlotTag.ToString());
				return;
			}

			SlotData->ItemWidget->RemoveFromParent();
			SlotData->Reset();
		}
	}
}



