// Copyright 2024 out of sCope team - Michał Ogiński

#include "UI/Inventory/SubWidgets/ObsidianItemSlot_Equipment.h"

// ~ Core
#include "Components/SizeBox.h"

// ~ Project
#include "UI/Inventory/ObsidianEquipmentPanel.h"

void UObsidianItemSlot_Equipment::NativePreConstruct()
{
	Super::NativePreConstruct();

	if(Root_SizeBox)
	{
		Root_SizeBox->SetWidthOverride(SlotWidth);
		Root_SizeBox->SetHeightOverride(SlotHeight);
	}
}

void UObsidianItemSlot_Equipment::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if(EquipmentPanel)
	{
		EquipmentPanel->OnEquipmentSlotHover(this, true);
	}
}

void UObsidianItemSlot_Equipment::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	if(EquipmentPanel)
	{
		EquipmentPanel->OnEquipmentSlotHover(this, false);
	}
}

FReply UObsidianItemSlot_Equipment::NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if(EquipmentPanel && InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		EquipmentPanel->OnEquipmentSlotMouseButtonDown(this);
	}
	
	return Super::NativeOnPreviewMouseButtonDown(InGeometry, InMouseEvent);
}

void UObsidianItemSlot_Equipment::InitializeSlot(UObsidianEquipmentPanel* InEquipmentPanel, const FGameplayTag& InSlotTag)
{
	EquipmentPanel = InEquipmentPanel;
	
	if(!SlotTag.IsValid()) // Slot Tag has been already set in Blueprint
	{
		SlotTag = InSlotTag;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Attempting to set SlotTag but it has already been set."));
	}
}

void UObsidianItemSlot_Equipment::InitializeSlot(UObsidianEquipmentPanel* InEquipmentPanel)
{
	EquipmentPanel = InEquipmentPanel;
}
