// Copyright 2024 out of sCope team - Michał Ogiński

#include "Obsidian/Public/UI/Inventory/Slots/ObsidianItemSlot_Equipment.h"

// ~ Core
#include "Components/SizeBox.h"
#include "Components/Overlay.h"

// ~ Project
#include "Components/OverlaySlot.h"
#include "UI/Inventory/Items/ObsidianItem.h"
#include "UI/Inventory/Slots/ObsidianSlotBlockadeItem.h"

void UObsidianItemSlot_Equipment::NativePreConstruct()
{
	Super::NativePreConstruct();

	if(Root_SizeBox)
	{
		Root_SizeBox->SetWidthOverride(SlotWidth);
		Root_SizeBox->SetHeightOverride(SlotHeight);
	}
}

void UObsidianItemSlot_Equipment::InitializeSlot(const FGameplayTag& InSlotTag, const FGameplayTag& InSisterSlotTag)
{
	if(!SlotTag.IsValid()) // Slot Tag has been already set in Blueprint
	{
		SlotTag = InSlotTag;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Attempting to set SlotTag but it has already been set."));
	}

	if(!SisterSlotTag.IsValid()) // Slot Tag has been already set in Blueprint
	{
		SisterSlotTag = InSisterSlotTag;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Attempting to set SisterSlotTag but it has already been set."));
	}
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
	if(bIsBlocked == false)
	{
		OnEquipmentSlotHoverDelegate.Broadcast(this, true);
	}
}

void UObsidianItemSlot_Equipment::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	if(bIsBlocked == false)
	{
		OnEquipmentSlotHoverDelegate.Broadcast(this, false);
	}
}

FReply UObsidianItemSlot_Equipment::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if(InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		OnEquipmentSlotPressedDelegate.Broadcast(this);
	}
	
	return FReply::Handled();
}
