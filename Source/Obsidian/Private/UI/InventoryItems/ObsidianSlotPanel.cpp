// Copyright 2026 out of sCope team - intrxx

#include "UI/InventoryItems/ObsidianSlotPanel.h"

#include "Blueprint/WidgetTree.h"

#include "Obsidian/ObsidianLogCategories.h"
#include "Obsidian/Public/UI/InventoryItems/Slots/ObsidianSlot_ItemSlot.h"
#include "UI/InventoryItems/Items/ObsidianItem.h"
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
                                   const bool bInBlockSlot)
{
	OriginPosition = InPosition;
	ItemWidget = InItemWidget;
	bBlocked = bInBlockSlot;
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
	WidgetTree->ForEachWidget([this, &bSuccess](UWidget* InWidget)
		{
			if(UObsidianSlot_ItemSlot* EquipmentSlot = Cast<UObsidianSlot_ItemSlot>(InWidget))
			{
				EquipmentSlot->OnItemSlotHoverDelegate.AddUObject(this, &ThisClass::OnItemSlotHover);
				EquipmentSlot->OnItemSlotLeftButtonPressedDelegate.AddUObject(this, &ThisClass::OnItemSlotLeftMouseButtonDown);
				EquipmentSlot->OnItemSlotRightButtonPressedDelegate.AddUObject(this, &ThisClass::OnItemSlotRightMouseButtonDown);
				
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
		if (UObsidianSlot_ItemSlot* EquipmentSlot = SlotDataPair.Value.OwningSlot)
		{
			EquipmentSlot->OnItemSlotHoverDelegate.Clear();
			EquipmentSlot->OnItemSlotLeftButtonPressedDelegate.Clear();
			EquipmentSlot->OnItemSlotRightButtonPressedDelegate.Clear();
		}
	}
	
	Super::NativeDestruct();
}

TArray<UObsidianSlot_ItemSlot*> UObsidianSlotPanel::GetAllSlots() const
{
	TArray<UObsidianSlot_ItemSlot*> Slots;
	Slots.Reserve(SlotDataMap.Num());
	
	for (const TPair<FGameplayTag, FObsidianSlotData>& SlotDataPair : SlotDataMap)
	{
		if (UObsidianSlot_ItemSlot* EquipmentSlot = SlotDataPair.Value.OwningSlot)
		{
			Slots.Add(EquipmentSlot);
		}
	}

	return Slots;
}

UObsidianSlot_ItemSlot* UObsidianSlotPanel::GetSlotByPosition(const FGameplayTag& InAtSlotTag)
{
	if (const FObsidianSlotData* SlotData = SlotDataMap.Find(InAtSlotTag))
	{
		return SlotData->OwningSlot;
	}
	return nullptr;
}

const FObsidianSlotData* UObsidianSlotPanel::GetSlotDataAtGridPosition(const FGameplayTag& InAtSlotTag) const
{
	return SlotDataMap.Find(InAtSlotTag);
}

UObsidianItem* UObsidianSlotPanel::GetItemWidgetAtSlot(const FGameplayTag& InAtSlotTag) const
{
	const FObsidianSlotData* SlotData = SlotDataMap.Find(InAtSlotTag);
	if (SlotData && SlotData->IsOccupied())
	{
		return SlotData->ItemWidget;
	}
	return nullptr;
}

bool UObsidianSlotPanel::IsSlotOccupied(const FGameplayTag& InAtSlotTag) const
{
	if (const FObsidianSlotData* SlotData = SlotDataMap.Find(InAtSlotTag))
	{
		return SlotData->IsOccupied();
	}
	return false;
}

bool UObsidianSlotPanel::IsSlotBlocked(const FGameplayTag& InAtSlotTag) const
{
	if (const FObsidianSlotData* SlotData = SlotDataMap.Find(InAtSlotTag))
	{
		return SlotData->IsBlocked();
	}
	return false;
}

void UObsidianSlotPanel::AddItemWidget(UObsidianItem* InItemWidget, const FObsidianItemWidgetData& InItemWidgetData,
	const bool bInBlockSlot)
{
	if (InItemWidget == nullptr)
	{
		UE_LOG(ObLogItems, Error, TEXT("ItemWidget to Add Item Widget to Equipment Panel is invalid in [%hs]"),
			__FUNCTION__);
		return;
	}
	
	const FGameplayTag SlotTag = InItemWidgetData.ItemPosition.GetItemSlotTag();
	if (SlotTag.IsValid() == false)
	{
		UE_LOG(ObLogItems, Error, TEXT("Slot Tag to Add Item Widget to Equipment Panel is invalid in [%hs]"),
			__FUNCTION__);
		return;
	}

	UObsidianSlot_ItemSlot* EquipmentSlot = GetSlotByPosition(SlotTag);
	if (EquipmentSlot == nullptr)
	{
		UE_LOG(ObLogItems, Error, TEXT("Unable to find Equipment Slot for tag [%s] on Equipment Panel in [%hs]"),
			*SlotTag.ToString(), __FUNCTION__);
		return;
	}

	if (bInBlockSlot == false)
	{
		EquipmentSlot->AddItemToSlot(InItemWidget, InItemWidgetData.ItemSlotPadding);
		RegisterSlotItemWidget(InItemWidgetData.ItemPosition, InItemWidget, InItemWidgetData.bSwappedWithAnotherItem);
	}
	else
	{
		const FGameplayTag SisterSlotTag = EquipmentSlot->GetSisterSlotTag();
		if (SisterSlotTag.IsValid() == false)
		{
			UE_LOG(ObLogItems, Error, TEXT("Equipment Slot with Tag [%s] has no Sister Slot but the Item added"
								   " to it want to block it [%hs], please verify if the item and slots are set up correctly."),
				*EquipmentSlot->GetSlotTag().ToString(), __FUNCTION__);
			return;
		}

		UObsidianSlot_ItemSlot* SlotToBlock = GetSlotByPosition(SisterSlotTag);
		if (SlotToBlock == nullptr)
		{
			UE_LOG(ObLogItems, Error, TEXT("Unable to find Slot To Block for tag [%s] on Equipment Panel in [%hs]"),
				*SisterSlotTag.ToString(), __FUNCTION__);
			return;
		}

		FObsidianItemPosition ItemPosition;
		ConstructItemPosition(ItemPosition, SisterSlotTag);
		SlotToBlock->AddBlockadeItemToSlot(InItemWidget, InItemWidgetData.ItemSlotPadding);
		SlotToBlock->SetSlotState(EObsidianItemSlotState::Blocked, EObsidianItemSlotStatePriority::TakePriority);
		RegisterSlotItemWidget(ItemPosition, InItemWidget, false, true,
			InItemWidgetData.ItemPosition);
	}
}

void UObsidianSlotPanel::OnItemSlotHover(UObsidianSlot_ItemSlot* InAffectedSlot, const bool bInEntered)
{
	if(InventoryItemsWidgetController == nullptr)
	{
		UE_LOG(ObLogItems, Error, TEXT("InventoryItemsWidgetController is invalid in [%hs]."), __FUNCTION__)
		return;
	}

	if (InAffectedSlot == nullptr)
	{
		UE_LOG(ObLogItems, Error, TEXT("AffectedSlot is invalid in [%hs]."), __FUNCTION__)
		return;
	}
	
	const FGameplayTag SlotTag = InAffectedSlot->GetSlotTag();
	check(SlotTag.IsValid());

	const FObsidianSlotData* SlotData = GetSlotDataAtGridPosition(SlotTag);
	if (SlotData == nullptr)
	{
		UE_LOG(ObLogItems, Error, TEXT("Could not find SlotData for tag [%s] in [%hs]."),
			*SlotTag.ToString(), __FUNCTION__)
		return;
	}
	
	const bool bCanInteract = InventoryItemsWidgetController->CanInteractWithSlots(PanelOwner);
	const bool IsSlotOccupied = SlotData->IsOccupied();
	const bool IsSlotBlocked = SlotData->IsBlocked();
	const bool IsSlotEmpty = IsSlotBlocked == false && IsSlotOccupied == false;
	
	if (bInEntered)
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
			InAffectedSlot->SetSlotState(EObsidianItemSlotState::RedLight, EObsidianItemSlotStatePriority::Low);
			return;
		}
	
		if(InventoryItemsWidgetController->IsDraggingAnItem() == false)
		{
			InAffectedSlot->SetSlotState(EObsidianItemSlotState::Selected, EObsidianItemSlotStatePriority::Low);
			return;
		}
		
		FObsidianItemPosition ItemPosition;
		ConstructItemPosition(ItemPosition, SlotTag);
		
		const bool bCanPlace = InventoryItemsWidgetController->CanPlaceDraggedItemAtPosition(ItemPosition, PanelOwner);
		
		const EObsidianItemSlotState SlotState = bCanPlace ? EObsidianItemSlotState::GreenLight
			: EObsidianItemSlotState::RedLight;
		InAffectedSlot->SetSlotState(SlotState, EObsidianItemSlotStatePriority::Low);
	}
	else
	{
		if (IsSlotEmpty == false)
		{
			InventoryItemsWidgetController->HandleUnhoveringItem(SlotData->OriginPosition);
		}

		if (bCanInteract)
		{
			InAffectedSlot->SetSlotState(EObsidianItemSlotState::Neutral, EObsidianItemSlotStatePriority::Low);
		}
	}
}

void UObsidianSlotPanel::OnItemSlotLeftMouseButtonDown(const UObsidianSlot_ItemSlot* InAffectedSlot,
	const FObsidianItemInteractionFlags& InInteractionFlags)
{
	if(InventoryItemsWidgetController == nullptr)
	{
		UE_LOG(ObLogItems, Error, TEXT("InventoryItemsWidgetController is invalid in [%hs]."), __FUNCTION__)
		return;
	}

	if (InAffectedSlot == nullptr)
	{
		UE_LOG(ObLogItems, Error, TEXT("AffectedSlot is invalid in [%hs]."), __FUNCTION__)
		return;
	}
	
	const FGameplayTag SlotTag = InAffectedSlot->GetSlotTag();
	check(SlotTag.IsValid());

	const FObsidianSlotData* SlotData = GetSlotDataAtGridPosition(SlotTag);
	if (SlotData == nullptr)
	{
		UE_LOG(ObLogItems, Error, TEXT("Could not find SlotData for tag [%s] in [%hs]."),
			*SlotTag.ToString(), __FUNCTION__)
		return;
	}

	FObsidianItemPosition ItemPosition;
	ConstructItemPosition(ItemPosition, SlotTag);
	
	FObsidianItemInteractionData InteractionData;
	InteractionData.InteractionFlags = InInteractionFlags;
	
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
		InventoryItemsWidgetController->HandleLeftClickingOnSlot(ItemPosition, InteractionData, PanelOwner);
	}
}

void UObsidianSlotPanel::OnItemSlotRightMouseButtonDown(const UObsidianSlot_ItemSlot* InAffectedSlot,
	const FObsidianItemInteractionFlags& InInteractionFlags)
{
	if(InventoryItemsWidgetController == nullptr)
	{
		UE_LOG(ObLogItems, Error, TEXT("InventoryItemsWidgetController is invalid in [%hs]."), __FUNCTION__)
		return;
	}

	if (InAffectedSlot == nullptr)
	{
		UE_LOG(ObLogItems, Error, TEXT("AffectedSlot is invalid in [%hs]."), __FUNCTION__)
		return;
	}

	const FGameplayTag SlotTag = InAffectedSlot->GetSlotTag();
	check(SlotTag.IsValid());

	const FObsidianSlotData* SlotData = GetSlotDataAtGridPosition(SlotTag);
	if (SlotData == nullptr)
	{
		UE_LOG(ObLogItems, Error, TEXT("Could not find SlotData for tag [%s] in [%hs]."),
			*SlotTag.ToString(), __FUNCTION__)
		return;
	}

	FObsidianItemPosition ItemPosition;
	ConstructItemPosition(ItemPosition, SlotTag);
	
	FObsidianItemInteractionData InteractionData;
	InteractionData.InteractionFlags = InInteractionFlags;
	
	if (SlotData && SlotData->IsOccupied())
	{
		InteractionData.ItemWidget = SlotData->ItemWidget;
		InventoryItemsWidgetController->HandleRightClickingOnItem(SlotData->OriginPosition, InteractionData, PanelOwner);
	}
	else
	{
		InventoryItemsWidgetController->HandleRightClickingOnSlot(ItemPosition, InteractionData, PanelOwner);
	}
}

void UObsidianSlotPanel::ConstructItemPosition(FObsidianItemPosition& OutItemPosition, const FGameplayTag& InSlotTagOverride) const
{
	if (PanelOwner == EObsidianPanelOwner::Equipment)
	{
		OutItemPosition = FObsidianItemPosition(InSlotTagOverride);
	}
	else if (PanelOwner == EObsidianPanelOwner::PlayerStash)
	{
		OutItemPosition = FObsidianItemPosition(InSlotTagOverride, StashTag);
	}
}

void UObsidianSlotPanel::HandleItemRemoved(const FObsidianItemWidgetData& InItemWidgetData)
{
	const FGameplayTag SlotToClearTag = InItemWidgetData.ItemPosition.GetItemSlotTag();
	if (ensureMsgf(SlotToClearTag.IsValid(), TEXT("SlotToClearTag is invalid in [%hs]. "), __FUNCTION__))
	{
		UnregisterSlotItemWidget(SlotToClearTag);

		if (InItemWidgetData.bDoesBlockSisterSlot)
		{
			if (const FObsidianSlotData* SlotData = SlotDataMap.Find(SlotToClearTag))
			{
				if (const UObsidianSlot_ItemSlot* OwningSlot = SlotData->OwningSlot)
				{
					const FGameplayTag NextSlotTagToClear = OwningSlot->GetSisterSlotTag();
					if (UObsidianSlot_ItemSlot* SisterSlot = GetSlotByPosition(NextSlotTagToClear))
					{
						UnregisterSlotItemWidget(NextSlotTagToClear);
						SisterSlot->ResetSlotState();
					}
				}
			}
		}
	}
	
}

void UObsidianSlotPanel::HandleItemChanged(const FObsidianItemWidgetData& InItemWidgetData)
{
	const FGameplayTag AtSlot = InItemWidgetData.ItemPosition.GetItemSlotTag();
	if(UObsidianItem* ItemWidget = GetItemWidgetAtSlot(AtSlot))
	{
		if (InItemWidgetData.bUpdateStacks)
		{
			ItemWidget->OverrideCurrentStackCount(InItemWidgetData.StackCount);
		}
		else if (InItemWidgetData.bGeneralItemUpdate)
		{
			if (HighlightedItems.Remove(ItemWidget))
			{
				ItemWidget->ResetHighlight();

				if (InventoryItemsWidgetController)
				{
					FObsidianItemInteractionData InteractionData;
					InteractionData.ItemWidget = ItemWidget;
					InventoryItemsWidgetController->HandleHoveringOverItem(InItemWidgetData.ItemPosition, InteractionData,
						PanelOwner);
				}
			}
		}
	}
}

void UObsidianSlotPanel::HandleHighlightingItems(const TArray<FObsidianItemPosition>& InItemsToHighlight)
{
	HighlightedItems.Reserve(InItemsToHighlight.Num());
	
	for (const FObsidianItemPosition& ItemPosition : InItemsToHighlight)
	{
		if (UObsidianItem* ItemWidget = GetItemWidgetAtSlot(ItemPosition.GetItemSlotTag()))
		{
			ItemWidget->HighlightItem();
			HighlightedItems.Add(ItemWidget);
		}
	}
}

void UObsidianSlotPanel::ClearUsableItemHighlight()
{
	for (UObsidianItem* HighlightedWidget : HighlightedItems)
	{
		if (HighlightedWidget)
		{
			HighlightedWidget->ResetHighlight();
		}
	}
	
	HighlightedItems.Empty();
}

void UObsidianSlotPanel::RegisterSlotItemWidget(const FObsidianItemPosition& InItemPosition, UObsidianItem* InItemWidget,
                                                const bool bInSwappedWithAnother, const bool bInBlocksSlot, const FObsidianItemPosition& InItemOriginPosition)
{
	if (ensureMsgf(InItemWidget && InItemPosition.IsValid(), TEXT("ItemWidget or ItemPosition are invalid in [%hs]. "),
		__FUNCTION__))
	{
		if(bInSwappedWithAnother)
		{
			UnregisterSlotItemWidget(InItemPosition.GetItemSlotTag());
		}
		
		if (FObsidianSlotData* SlotData = SlotDataMap.Find(InItemPosition.GetItemSlotTag()))
		{
			check(SlotData->IsOccupied() == false);
			const FObsidianItemPosition OriginPosition = bInBlocksSlot ? InItemOriginPosition : InItemPosition;
			SlotData->AddNewItem(OriginPosition, InItemWidget, bInBlocksSlot);
		}
	}
}

void UObsidianSlotPanel::UnregisterSlotItemWidget(const FGameplayTag& InSlotTag)
{
	if  (ensureMsgf(InSlotTag.IsValid(), TEXT("SlotTag is invalid in [%hs]. "), __FUNCTION__))
	{
		if (FObsidianSlotData* SlotData = SlotDataMap.Find(InSlotTag))
		{
			check(SlotData->IsOccupied());

			UObsidianItem* SlottedItemWidget = SlotData->ItemWidget;
			if (SlottedItemWidget == nullptr)
			{
				UE_LOG(ObLogItems, Error, TEXT("Trying to remove ItemWidget from [%s], but the ItemWidget is invalid!"),
					*InSlotTag.ToString());
				return;
			}

			SlottedItemWidget->RemoveFromParent();
			SlotData->Reset();
		}
	}
}



