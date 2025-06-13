// Copyright 2024 out of sCope team - Michał Ogiński

#include "UI/Inventory/SubWidgets/ObsidianItemSlot_Equipment.h"

// ~ Core
#include "Components/SizeBox.h"
#include "Components/Overlay.h"

// ~ Project
#include "Components/OverlaySlot.h"
#include "UI/Inventory/ObsidianEquipmentPanel.h"
#include "UI/Inventory/ObsidianItem.h"
#include "UI/Inventory/ObsidianSlotBlockadeItem.h"

void UObsidianItemSlot_Equipment::NativePreConstruct()
{
	Super::NativePreConstruct();

	if(Root_SizeBox)
	{
		Root_SizeBox->SetWidthOverride(SlotWidth);
		Root_SizeBox->SetHeightOverride(SlotHeight);
	}
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

void UObsidianItemSlot_Equipment::AddItemToSlot(UObsidianItem* InItemWidget, const float ItemSlotPadding)
{
	if(Main_Overlay)
	{
		UOverlaySlot* ItemSlot = Main_Overlay->AddChildToOverlay(InItemWidget);
		ItemSlot->SetHorizontalAlignment(HAlign_Center);
		ItemSlot->SetVerticalAlignment(VAlign_Center);
		
		const float ItemPadding = SlottedItemAdditionalPadding + ItemSlotPadding;
		ItemSlot->SetPadding(ItemPadding);
	}
}

void UObsidianItemSlot_Equipment::AddItemToSlot(UObsidianSlotBlockadeItem* InItemWidget, const float ItemSlotPadding)
{
	if(Main_Overlay)
	{
		bIsBlocked = true;
		
		UOverlaySlot* ItemSlot = Main_Overlay->AddChildToOverlay(InItemWidget);
		ItemSlot->SetHorizontalAlignment(HAlign_Center);
		ItemSlot->SetVerticalAlignment(VAlign_Center);

		const float ItemPadding = SlottedItemAdditionalPadding + ItemSlotPadding;
		ItemSlot->SetPadding(ItemPadding);
	}
}

void UObsidianItemSlot_Equipment::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if(EquipmentPanel && bIsBlocked == false)
	{
		EquipmentPanel->OnEquipmentSlotHover(this, true);
	}
}

void UObsidianItemSlot_Equipment::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	if(EquipmentPanel && bIsBlocked == false)
	{
		EquipmentPanel->OnEquipmentSlotHover(this, false);
	}
}

FReply UObsidianItemSlot_Equipment::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if(EquipmentPanel && InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		EquipmentPanel->OnEquipmentSlotMouseButtonDown(this);
	}
	
	return FReply::Handled();
}
