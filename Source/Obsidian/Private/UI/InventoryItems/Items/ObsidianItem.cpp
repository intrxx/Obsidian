// Copyright 2026 out of sCope team - intrxx

#include "UI/InventoryItems/Items/ObsidianItem.h"

#include "CommonTextBlock.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"

#include "ObsidianTypes/ItemTypes/ObsidianItemTypes.h"


void UObsidianItem::NativeConstruct()
{
	Super::NativeConstruct();

	SetVisibility(ESlateVisibility::HitTestInvisible);
	
	if(Highlight_Image)
	{
		Highlight_Image->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UObsidianItem::InitializeItemWidget(const FIntPoint& InItemGridSpan, UTexture2D* InItemImage, const int32 InCurrentStack)
{
	InternalStacks = InCurrentStack;
	
	Root_SizeBox->SetWidthOverride(InItemGridSpan.X * ObsidianInventoryItemsStatics::InventorySlotSize.X);
	Root_SizeBox->SetHeightOverride(InItemGridSpan.Y * ObsidianInventoryItemsStatics::InventorySlotSize.Y);
	Item_Image->SetBrushFromTexture(InItemImage);
	
	if(InCurrentStack == 0)
	{
		StackCount_TextBlock->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}
	
	StackCount_TextBlock->SetText(FText::AsNumber(InCurrentStack));
	StackCount_TextBlock->SetVisibility(ESlateVisibility::Visible);
}

void UObsidianItem::InitializeItemWidget(const FIntPoint& InItemGridSpan, UTexture2D* InItemImage, const bool bInIsForSwapSlot)
{
	const float SlotSizeMultiplier = bInIsForSwapSlot == true ? SwapSlotSizeMultiplier : 1.0f;
		
	const float WidthOverride = (InItemGridSpan.X * ObsidianInventoryItemsStatics::InventorySlotSize.X) * SlotSizeMultiplier;
	const float HeightOverride = (InItemGridSpan.Y * ObsidianInventoryItemsStatics::InventorySlotSize.Y) * SlotSizeMultiplier;
	Root_SizeBox->SetWidthOverride(WidthOverride);
	Root_SizeBox->SetHeightOverride(HeightOverride);

	FSlateBrush Brush;
	Brush.SetImageSize(FVector2D(WidthOverride, HeightOverride));
	Brush.SetResourceObject(InItemImage);
	Item_Image->SetBrush(Brush);
	
	StackCount_TextBlock->SetVisibility(ESlateVisibility::Collapsed);
}

void UObsidianItem::AddCurrentStackCount(const int32 InStackCountToAdd)
{
	if(InStackCountToAdd <= 0)
	{
		return;
	}
	
	InternalStacks += InStackCountToAdd;
	if(InternalStacks <= 0)
	{
		StackCount_TextBlock->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}
	StackCount_TextBlock->SetText(FText::AsNumber(InternalStacks));
	StackCount_TextBlock->SetVisibility(ESlateVisibility::Visible);
}

void UObsidianItem::OverrideCurrentStackCount(const int32 InNewStackCount)
{
	if(InNewStackCount <= 0)
	{
		return;
	}
	InternalStacks = InNewStackCount;
	
	StackCount_TextBlock->SetText(FText::AsNumber(InNewStackCount));
	StackCount_TextBlock->SetVisibility(ESlateVisibility::Visible);
}

FSlateBrush UObsidianItem::GetItemImage() const
{
	if(Item_Image)
	{
		return Item_Image->GetBrush();
	}
	return FSlateBrush();
}

FVector2D UObsidianItem::GetItemWidgetSize() const
{
	return FVector2D(Root_SizeBox->GetWidthOverride(), Root_SizeBox->GetHeightOverride());
}

void UObsidianItem::SetBlockadeItemProperties()
{
	SetRenderOpacity(BlockingItemOpacity);
}

void UObsidianItem::ResetBlockadeItemProperties()
{
	SetRenderOpacity(1.0f);
}

void UObsidianItem::SetUsingItemProperties()
{
	SetRenderOpacity(UsingItemOpacity);
}

void UObsidianItem::ResetUsingItemProperties()
{
	SetRenderOpacity(1.0f);
}

void UObsidianItem::HighlightItem()
{
	if(Highlight_Image)
	{
		Highlight_Image->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

void UObsidianItem::ResetHighlight()
{
	if(Highlight_Image)
	{
		Highlight_Image->SetVisibility(ESlateVisibility::Hidden);
	}
}
