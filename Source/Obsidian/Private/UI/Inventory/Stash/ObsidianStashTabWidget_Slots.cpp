// Copyright 2024 out of sCope team - Michał Ogiński

#include "UI/Inventory/Stash/ObsidianStashTabWidget_Slots.h"

// ~ Core
#include "Blueprint/WidgetTree.h"

// ~ Project
#include "UI/Inventory/Items/ObsidianItem.h"
#include "UI/Inventory/Slots/ObsidianItemSlot_Equipment.h"
#include "UI/WidgetControllers/ObsidianInventoryItemsWidgetController.h"

void UObsidianStashTabWidget_Slots::InitializeStashTab(UObsidianInventoryItemsWidgetController* InInventoryItemsWidgetController, UObsidianPlayerStashWidget* InOwningStashWidget, const FGameplayTag& InStashTabTag)
{
	if (InInventoryItemsWidgetController && InOwningStashWidget && InStashTabTag.IsValid())
	{
		InventoryItemsController = InInventoryItemsWidgetController;
		OwningStashWidget = InOwningStashWidget;
		StashTabTag = InStashTabTag;
	
		WidgetTree->ForEachWidget([this](UWidget* Widget)
			{
				if(UObsidianItemSlot_Equipment* EquipmentSlot = Cast<UObsidianItemSlot_Equipment>(Widget))
				{
					EquipmentSlot->OnEquipmentSlotHoverDelegate.AddUObject(this, &ThisClass::OnStashSlotHover);
					EquipmentSlot->OnEquipmentSlotPressedDelegate.AddUObject(this, &ThisClass::OnStashSlotMouseButtonDown);
					EquipmentSlots.Add(EquipmentSlot);
				}
			});
	}
}

void UObsidianStashTabWidget_Slots::AddItemToStash(UObsidianItem* InItemWidget, const float ItemSlotPadding)
{
	if(InItemWidget == nullptr)
	{
		return;
	}

	const FObsidianItemPosition ItemPosition = InItemWidget->GetItemPosition();
	if (UObsidianItemSlot_Equipment* EquipmentSlot = FindEquipmentSlotForTag(ItemPosition.GetItemSlotTag()))
	{
		EquipmentSlot->AddItemToSlot(InItemWidget, ItemSlotPadding);
	}
}

UObsidianItemSlot_Equipment* UObsidianStashTabWidget_Slots::FindEquipmentSlotForTag(const FGameplayTag& Tag) const
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

void UObsidianStashTabWidget_Slots::OnStashSlotHover(const UObsidianItemSlot_Equipment* AffectedSlot, const bool bEntered) const
{
	if (InventoryItemsController == nullptr || AffectedSlot == nullptr)
	{
		return;
	}

	if (bEntered == false)
	{
		AffectedSlot->SetSlotState(ISS_Neutral);
		return;
	}

	if(InventoryItemsController->CanInteractWithPlayerStash() == false)
	{
		AffectedSlot->SetSlotState(ISS_RedLight);
		return;
	}

	if(InventoryItemsController->IsDraggingAnItem() == false)
	{
		AffectedSlot->SetSlotState(ISS_Selected);
		return;
	}
		
	const bool bCanPlace = InventoryItemsController->CanPlaceItemAtStashSlot(FObsidianItemPosition(AffectedSlot->GetSlotTag(), StashTabTag));
	const EObsidianItemSlotState SlotState = bCanPlace ? ISS_GreenLight : ISS_RedLight;
	AffectedSlot->SetSlotState(SlotState);
}

void UObsidianStashTabWidget_Slots::OnStashSlotMouseButtonDown(const UObsidianItemSlot_Equipment* AffectedSlot, const bool bShiftDown) const
{
	if (InventoryItemsController && AffectedSlot)
	{
		InventoryItemsController->RequestAddingItemToStashTab(FObsidianItemPosition(AffectedSlot->GetSlotTag(), StashTabTag), bShiftDown);
	}
}
