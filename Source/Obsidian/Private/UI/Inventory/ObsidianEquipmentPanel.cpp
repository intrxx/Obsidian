// Copyright 2024 out of sCope team - Michał Ogiński

#include "UI/Inventory/ObsidianEquipmentPanel.h"

// ~ Core

// ~ Project
#include "Blueprint/WidgetTree.h"
#include "UI/Inventory/SubWidgets/ObsidianItemSlot_Equipment.h"

void UObsidianEquipmentPanel::InitializeEquipmentPanel(UObsidianInventory* InOwningInventory)
{
	OwningInventory = InOwningInventory;
	
	WidgetTree->ForEachWidget([this](UWidget* Widget)
		{
			if(UObsidianItemSlot_Equipment* EquipmentSlot = Cast<UObsidianItemSlot_Equipment>(Widget))
			{
				EquipmentSlot->InitializeSlot(this);
				EquipmentSlots.Add(EquipmentSlot);
			}
		});
}

UObsidianItemSlot_Equipment* UObsidianEquipmentPanel::FindEquipmentSlotForTag(const FGameplayTag& Tag) const
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

void UObsidianEquipmentPanel::OnEquipmentSlotHover(UObsidianItemSlot_Equipment* AffectedSlot, const bool bEntered) const
{
	if (OwningInventory.IsValid() == false)
	{
		return;
	}
	
	if(bEntered)
	{
		if(OwningInventory->IsPlayerDraggingItem() == false)
		{
			const EObsidianItemSlotState SlotState = OwningInventory->CanInteractWithEquipment() ? ISS_Selected : ISS_RedLight;
			AffectedSlot->SetSlotState(SlotState);
			return;
		}
		
		const bool bCanEquip = OwningInventory->CanEquipDraggedItem(AffectedSlot->GetSlotTag());
		const EObsidianItemSlotState SlotState = bCanEquip ? ISS_GreenLight : ISS_RedLight;
		AffectedSlot->SetSlotState(SlotState);
		return;
	}
	AffectedSlot->SetSlotState(ISS_Neutral);
}

void UObsidianEquipmentPanel::OnEquipmentSlotMouseButtonDown(const UObsidianItemSlot_Equipment* AffectedSlot) const
{
	OnEquipmentSlotPressedDelegate.Broadcast(AffectedSlot->GetSlotTag());
}

