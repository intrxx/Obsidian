// Copyright 2024 out of sCope team - Michał Ogiński

#include "UI/Inventory/ObsidianEquipmentPanel.h"

// ~ Core
#include "Blueprint/WidgetTree.h"

// ~ Project
#include "UI/Inventory/ObsidianInventory.h"
#include "Obsidian/Public/UI/Inventory/Slots/ObsidianItemSlot_Equipment.h"

void UObsidianEquipmentPanel::InitializeEquipmentPanel(UObsidianInventory* InOwningInventory)
{
	OwningInventory = InOwningInventory;
	
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

UObsidianItemSlot_Equipment* UObsidianEquipmentPanel::FindEquipmentSlotWidgetForTag(const FGameplayTag& Tag) const
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

TArray<UObsidianItemSlot_Equipment*> UObsidianEquipmentPanel::GetSlotWidgets() const
{
	return EquipmentSlots;
}

void UObsidianEquipmentPanel::OnEquipmentSlotHover(UObsidianItemSlot_Equipment* AffectedSlot, const bool bEntered)
{
	if (OwningInventory == nullptr || OwningInventory.IsValid() == false)
	{
		return;
	}
	
	if (bEntered == false)
	{
		AffectedSlot->SetSlotState(EObsidianItemSlotState::Neutral, EObsidianItemSlotStatePriority::Low);
		return;
	}
	
	if(OwningInventory->CanInteractWithEquipment() == false)
	{
		AffectedSlot->SetSlotState(EObsidianItemSlotState::RedLight, EObsidianItemSlotStatePriority::Low);
		return;
	}

	if(OwningInventory->IsPlayerDraggingItem() == false)
	{
		AffectedSlot->SetSlotState(EObsidianItemSlotState::Selected, EObsidianItemSlotStatePriority::Low);
		return;
	}
		
	const bool bInteractionSuccess = OwningInventory->CanEquipDraggedItem(AffectedSlot->GetSlotTag());
	const EObsidianItemSlotState SlotState = bInteractionSuccess ? EObsidianItemSlotState::GreenLight : EObsidianItemSlotState::RedLight;
	AffectedSlot->SetSlotState(SlotState, EObsidianItemSlotStatePriority::Low);
}

void UObsidianEquipmentPanel::OnEquipmentSlotMouseButtonDown(const UObsidianItemSlot_Equipment* AffectedSlot, const bool bShiftDown) const
{
	if(OwningInventory.IsValid())
	{
		OwningInventory->RequestEquippingItem(AffectedSlot->GetSlotTag());
	}
}

