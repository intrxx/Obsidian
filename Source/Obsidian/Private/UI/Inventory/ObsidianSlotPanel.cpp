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

bool FObsidianSlotData::IsBlocked() const
{
	return bBlocked;
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
}

bool UObsidianSlotPanel::ConstructEquipmentPanel()
{
	PanelOwner = EObsidianPanelOwner::Equipment;
	return ConstructSlots();
}

bool UObsidianSlotPanel::ConstructStashPanel(const FGameplayTag& InStashTabTag)
{
	if (InStashTabTag.IsValid() == false)
	{
		return false;
	}

	StashTag = InStashTabTag;
	PanelOwner = EObsidianPanelOwner::PlayerStash;
	return ConstructSlots();
}

bool UObsidianSlotPanel::ConstructSlots()
{
	bool bSuccess = false;
	WidgetTree->ForEachWidget([this, &bSuccess](UWidget* Widget)
		{
			if(UObsidianItemSlot_Equipment* EquipmentSlot = Cast<UObsidianItemSlot_Equipment>(Widget))
			{
				EquipmentSlot->OnEquipmentSlotHoverDelegate.AddUObject(this, &ThisClass::OnEquipmentSlotHover);
				EquipmentSlot->OnEquipmentSlotPressedDelegate.AddUObject(this, &ThisClass::OnEquipmentSlotMouseButtonDown);
				
				FObsidianSlotData NewData;
				NewData.OwningSlot = EquipmentSlot;
				SlotDataMap.Add(EquipmentSlot->GetSlotTag(), NewData);
				bSuccess = true;
			}
		});
	return bSuccess;
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
		return SlotData->IsBlocked();
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

		FObsidianItemPosition ItemPosition;
		ConstructItemPosition(ItemPosition, SisterSlotTag);
		SlotToBlock->AddBlockadeItemToSlot(ItemWidget, ItemWidgetData.ItemSlotPadding);
		SlotToBlock->SetSlotState(EObsidianItemSlotState::Blocked, EObsidianItemSlotStatePriority::TakePriority);
		RegisterSlotItemWidget(ItemPosition, ItemWidget, false, true,
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
	
	const bool bCanInteract = InventoryItemsWidgetController->CanInteractWithSlots(PanelOwner);
	const bool IsSlotOccupied = SlotData->IsOccupied();
	const bool IsSlotBlocked = SlotData->IsBlocked();
	const bool IsSlotEmpty = IsSlotBlocked == false && IsSlotOccupied == false;
	
	if (bEntered)
	{
		if (IsSlotEmpty == false)
		{
			if (UObsidianItem* HoverOverItemWidget = SlotData->ItemWidget)
			{
				FObsidianItemInteractionData InteractionData;
				InteractionData.ItemWidget = HoverOverItemWidget;
				InventoryItemsWidgetController->HandleHoveringOverItem(SlotData->OriginPosition, InteractionData,
					PanelOwner);	
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
		
		FObsidianItemPosition ItemPosition;
		ConstructItemPosition(ItemPosition, SlotTag);
		
		const bool bCanPlace = InventoryItemsWidgetController->CanPlaceDraggedItemAtPosition(ItemPosition, PanelOwner);
		
		const EObsidianItemSlotState SlotState = bCanPlace ? EObsidianItemSlotState::GreenLight
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

	FObsidianItemPosition ItemPosition;
	ConstructItemPosition(ItemPosition, SlotTag);
	
	FObsidianItemInteractionData InteractionData;
	InteractionData.InteractionFlags = InteractionFlags;
	
	if (SlotData->IsOccupied())
	{
		if (SlotData->IsBlocked())
		{
			InteractionData.InteractionFlags.bInteractWithSisterSlottedItem = true;
			InteractionData.InteractionTargetPositionOverride = SlotData->OriginPosition;
		}
		
		InventoryItemsWidgetController->HandleLeftClickingOnItem(ItemPosition, InteractionData, PanelOwner);
	}
	else
	{
		InventoryItemsWidgetController->RequestAddingItem(ItemPosition, InteractionData, PanelOwner);
	}
}

void UObsidianSlotPanel::ConstructItemPosition(FObsidianItemPosition& ItemPosition, const FGameplayTag& SlotTagOverride) const
{
	if (PanelOwner == EObsidianPanelOwner::Equipment)
	{
		ItemPosition = FObsidianItemPosition(SlotTagOverride);
	}
	else if (PanelOwner == EObsidianPanelOwner::PlayerStash)
	{
		ItemPosition = FObsidianItemPosition(SlotTagOverride, StashTag);
	}
}

void UObsidianSlotPanel::HandleItemRemoved(const FObsidianItemWidgetData& ItemWidgetData)
{
	const FGameplayTag SlotToClearTag = ItemWidgetData.ItemPosition.GetItemSlotTag();
	if (ensureMsgf(SlotToClearTag.IsValid(), TEXT("SlotToClearTag is invalid in [%hs]. "), __FUNCTION__))
	{
		UnregisterSlotItemWidget(SlotToClearTag);

		if (ItemWidgetData.bDoesBlockSisterSlot)
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

void UObsidianSlotPanel::HandleItemChanged(const FObsidianItemWidgetData& ItemWidgetData)
{
	const FGameplayTag AtSlot = ItemWidgetData.ItemPosition.GetItemSlotTag();
	if(UObsidianItem* ItemWidget = GetItemWidgetAtSlot(AtSlot))
	{
		ItemWidget->OverrideCurrentStackCount(ItemWidgetData.StackCount);
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

			UObsidianItem* SlottedItemWidget = SlotData->ItemWidget;
			if (SlottedItemWidget == nullptr)
			{
				UE_LOG(LogTemp, Error, TEXT("Trying to remove ItemWidget from [%s], but the ItemWidget is invalid!"),
					*SlotTag.ToString());
				return;
			}

			SlottedItemWidget->RemoveFromParent();
			SlotData->Reset();
		}
	}
}



