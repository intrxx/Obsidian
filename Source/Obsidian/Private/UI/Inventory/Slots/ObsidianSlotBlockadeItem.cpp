// Copyright 2024 out of sCope team - Michał Ogiński

#include "UI/Inventory/Slots/ObsidianSlotBlockadeItem.h"

// ~ Core
#include "Components/Image.h"
#include "Components/SizeBox.h"

// ~ Project
#include "ObsidianTypes/ItemTypes/ObsidianItemTypes.h"
#include "Obsidian/Public/UI/Inventory/Slots/ObsidianItemSlot_Equipment.h"

void UObsidianSlotBlockadeItem::NativeConstruct()
{
	Super::NativeConstruct();

	SetRenderOpacity(ItemOpacity);
}

void UObsidianSlotBlockadeItem::NativeDestruct()
{
	if(OwningSlot)
	{
		OwningSlot->SetSlotState(EObsidianItemSlotState::Neutral, EObsidianItemSlotStatePriority::TakePriority);
	}
	
	Super::NativeDestruct();
}

void UObsidianSlotBlockadeItem::InitializeItemWidget(const FIntPoint& ItemGridSpan, UTexture2D* ItemImage, const bool bIsForSwapSlot)
{
	const float SlotSizeMultiplier = bIsForSwapSlot == true ? SwapSlotSizeMultiplier : 1.0f;
	
	const float WidthOverride = (ItemGridSpan.X * ObsidianInventoryItemsStatics::InventorySlotSize.X) * SlotSizeMultiplier;
	const float HeightOverride = (ItemGridSpan.Y * ObsidianInventoryItemsStatics::InventorySlotSize.Y) * SlotSizeMultiplier;
	Root_SizeBox->SetWidthOverride(WidthOverride);
	Root_SizeBox->SetHeightOverride(HeightOverride);
	
	FSlateBrush Brush;
	Brush.SetImageSize(FVector2D(WidthOverride, HeightOverride));
	Brush.SetResourceObject(ItemImage);
	Item_Image->SetBrush(Brush);
}




