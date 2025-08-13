// Copyright 2024 out of sCope team - Michał Ogiński

#include "UI/Inventory/Stash/ObsidianStashTabWidget_Slots.h"

// ~ Core
#include "Blueprint/WidgetTree.h"

// ~ Project
#include "UI/Inventory/Slots/ObsidianItemSlot_Equipment.h"
#include "UI/Inventory/ObsidianPlayerStashWidget.h"

void UObsidianStashTabWidget_Slots::InitializeStashTab(UObsidianPlayerStashWidget* InOwningStashWidget)
{
	OwningStashWidget = InOwningStashWidget;
	
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

void UObsidianStashTabWidget_Slots::OnEquipmentSlotHover(const UObsidianItemSlot_Equipment* AffectedSlot, const bool bEntered) const
{
}

void UObsidianStashTabWidget_Slots::OnEquipmentSlotMouseButtonDown(const UObsidianItemSlot_Equipment* AffectedSlot) const
{
}
