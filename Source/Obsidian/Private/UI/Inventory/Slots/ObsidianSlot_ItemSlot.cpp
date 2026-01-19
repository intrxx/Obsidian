// Copyright 2024 out of sCope team - Michał Ogiński

#include "Obsidian/Public/UI/Inventory/Slots/ObsidianSlot_ItemSlot.h"

#include <Components/SizeBox.h>
#include <Components/Overlay.h>
#include <Components/OverlaySlot.h>

#include "UI/Inventory/Items/ObsidianItem.h"

void UObsidianSlot_ItemSlot::NativePreConstruct()
{
	Super::NativePreConstruct();

	if(Root_SizeBox)
	{
		Root_SizeBox->SetWidthOverride(SlotWidth);
		Root_SizeBox->SetHeightOverride(SlotHeight);
	}
}

void UObsidianSlot_ItemSlot::InitializeSlot(const FGameplayTag& InSlotTag, const FGameplayTag& InSisterSlotTag)
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

FGameplayTag UObsidianSlot_ItemSlot::GetSlotTag() const
{
	return SlotTag;
}

FGameplayTag UObsidianSlot_ItemSlot::GetSisterSlotTag() const
{
	return SisterSlotTag;
}

void UObsidianSlot_ItemSlot::AddItemToSlot(UObsidianItem* InItemWidget, const float ItemSlotPadding)
{
	if(ensure(Main_Overlay && InItemWidget))
	{
		UOverlaySlot* ItemSlot = Main_Overlay->AddChildToOverlay(InItemWidget);
		ItemSlot->SetHorizontalAlignment(HAlign_Center);
		ItemSlot->SetVerticalAlignment(VAlign_Center);
		
		const float ItemPadding = SlottedItemAdditionalPadding + ItemSlotPadding;
		ItemSlot->SetPadding(ItemPadding);
	}
}

void UObsidianSlot_ItemSlot::AddBlockadeItemToSlot(UObsidianItem* InItemWidget, const float ItemSlotPadding)
{
	if(ensure(Main_Overlay && InItemWidget))
	{
		InItemWidget->SetBlockadeItemProperties();
		
		UOverlaySlot* ItemSlot = Main_Overlay->AddChildToOverlay(InItemWidget);
		ItemSlot->SetHorizontalAlignment(HAlign_Center);
		ItemSlot->SetVerticalAlignment(VAlign_Center);

		const float ItemPadding = SlottedItemAdditionalPadding + ItemSlotPadding;
		ItemSlot->SetPadding(ItemPadding);
	}
}

void UObsidianSlot_ItemSlot::ResetSlotState()
{
	SetSlotState(EObsidianItemSlotState::Neutral, EObsidianItemSlotStatePriority::TakePriority);
}

void UObsidianSlot_ItemSlot::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	OnItemSlotHoverDelegate.Broadcast(this, true);
}

void UObsidianSlot_ItemSlot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	OnItemSlotHoverDelegate.Broadcast(this, false);
}

FReply UObsidianSlot_ItemSlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		FObsidianItemInteractionFlags ItemInteractionFlags;
		ItemInteractionFlags.bItemStacksInteraction = InMouseEvent.IsShiftDown();
		ItemInteractionFlags.bMoveBetweenNextOpenedWindow = InMouseEvent.IsControlDown();
		OnItemSlotLeftButtonPressedDelegate.Broadcast(this, ItemInteractionFlags);
	}
	else if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		FObsidianItemInteractionFlags ItemInteractionFlags;
		ItemInteractionFlags.bItemStacksInteraction = InMouseEvent.IsShiftDown();
		ItemInteractionFlags.bMoveBetweenNextOpenedWindow = InMouseEvent.IsControlDown();
		OnItemSlotRightButtonPressedDelegate.Broadcast(this, ItemInteractionFlags);
	}
	
	return FReply::Handled();
}
