// Copyright 2024 out of sCope team - Michał Ogiński

#include "Obsidian/Public/UI/Inventory/Slots/ObsidianItemSlot_Equipment.h"

#include <Components/SizeBox.h>
#include <Components/Overlay.h>
#include <Components/OverlaySlot.h>

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

void UObsidianItemSlot_Equipment::Reset()
{
	OnEquippedItemHoverDelegate.Clear();
	OnBlockedSlotHoverDelegate.Clear();
	OnEquippedItemLeftButtonPressedDelegate.Clear();
	OnBlockedSlotLeftButtonPressedDelegate.Clear();

	bContainsItem = false;
	ContainedItemPosition = FObsidianItemPosition();
	
	bIsBlocked = false;
	PrimaryItemPosition = FObsidianItemPosition();
}

FObsidianItemPosition UObsidianItemSlot_Equipment::GetPrimaryItemPosition() const
{
	return PrimaryItemPosition;
}

FGameplayTag UObsidianItemSlot_Equipment::GetSlotTag() const
{
	return SlotTag;
}

FGameplayTag UObsidianItemSlot_Equipment::GetSisterSlotTag() const
{
	return SisterSlotTag;
}

bool UObsidianItemSlot_Equipment::IsBlocked() const
{
	return bIsBlocked;
}

void UObsidianItemSlot_Equipment::SetIsBlocked(const bool bInIsBlocked)
{
	bIsBlocked = bInIsBlocked;
}

void UObsidianItemSlot_Equipment::AddItemToSlot(UObsidianItem* InItemWidget, const FObsidianItemPosition& InItemPosition,
	const float ItemSlotPadding)
{
	if(Main_Overlay)
	{
		bContainsItem = true;
		ContainedItemPosition = InItemPosition;
		
		UOverlaySlot* ItemSlot = Main_Overlay->AddChildToOverlay(InItemWidget);
		ItemSlot->SetHorizontalAlignment(HAlign_Center);
		ItemSlot->SetVerticalAlignment(VAlign_Center);
		
		const float ItemPadding = SlottedItemAdditionalPadding + ItemSlotPadding;
		ItemSlot->SetPadding(ItemPadding);
	}
}

void UObsidianItemSlot_Equipment::AddBlockadeItemToSlot(UObsidianSlotBlockadeItem* InItemWidget,
	const FObsidianItemPosition& InPrimaryItemPosition, const float ItemSlotPadding)
{
	if(Main_Overlay)
	{
		bIsBlocked = true;
		PrimaryItemPosition = InPrimaryItemPosition;
		
		UOverlaySlot* ItemSlot = Main_Overlay->AddChildToOverlay(InItemWidget);
		ItemSlot->SetHorizontalAlignment(HAlign_Center);
		ItemSlot->SetVerticalAlignment(VAlign_Center);

		const float ItemPadding = SlottedItemAdditionalPadding + ItemSlotPadding;
		ItemSlot->SetPadding(ItemPadding);
	}
}

void UObsidianItemSlot_Equipment::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (bIsBlocked)
	{
		OnBlockedSlotHoverDelegate.Broadcast(PrimaryItemPosition, true);
		return;
	}

	if (bContainsItem)
	{
		OnEquippedItemHoverDelegate.Broadcast(ContainedItemPosition, true);
	}
	else
	{
		OnEquipmentSlotHoverDelegate.Broadcast(this, true);
	}
}

void UObsidianItemSlot_Equipment::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	if (bIsBlocked)
	{
		OnBlockedSlotHoverDelegate.Broadcast(PrimaryItemPosition, false);
		return;
	}

	if (bContainsItem)
	{
		OnEquippedItemHoverDelegate.Broadcast(ContainedItemPosition, false);
	}
	else
	{
		OnEquipmentSlotHoverDelegate.Broadcast(this, false);
	}
}

FReply UObsidianItemSlot_Equipment::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (bIsBlocked)
	{
		if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
		{
			OnBlockedSlotLeftButtonPressedDelegate.Broadcast(this, PrimaryItemPosition, FObsidianItemInteractionFlags());
		}
		return FReply::Handled();
	}

	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		if (bContainsItem)
		{
			FObsidianItemInteractionFlags ItemInteractionFlags;
			ItemInteractionFlags.bItemStacksInteraction = InMouseEvent.IsShiftDown();
			ItemInteractionFlags.bMoveBetweenNextOpenedWindow = InMouseEvent.IsControlDown();
		
			OnEquippedItemLeftButtonPressedDelegate.Broadcast(this, ContainedItemPosition, ItemInteractionFlags);
		}
		else
		{
			OnEquipmentSlotPressedDelegate.Broadcast(this, InMouseEvent.IsShiftDown());
		}
	}
	
	return FReply::Handled();
}
