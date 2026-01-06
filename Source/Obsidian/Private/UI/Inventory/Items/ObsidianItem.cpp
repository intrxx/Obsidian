// Copyright 2024 out of sCope team - Michał Ogiński

#include "UI/Inventory/Items/ObsidianItem.h"

#include <CommonTextBlock.h>
#include <Components/Image.h>
#include <Components/SizeBox.h>


void UObsidianItem::NativeConstruct()
{
	Super::NativeConstruct();

	SetVisibility(ESlateVisibility::HitTestInvisible);
	
	if(Highlight_Image)
	{
		Highlight_Image->SetVisibility(ESlateVisibility::Hidden);
	}
}

FReply UObsidianItem::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if(InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		FObsidianItemInteractionFlags AddingItemFlags;
		AddingItemFlags.bItemStacksInteraction = InMouseEvent.IsShiftDown();
		AddingItemFlags.bMoveBetweenNextOpenedWindow = InMouseEvent.IsControlDown();
		
		OnItemLeftMouseButtonPressedDelegate.Broadcast(this, AddingItemFlags);
	}
	if(InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		OnItemRightMouseButtonPressedDelegate.Broadcast(this);
	}

	return FReply::Handled();
}

void UObsidianItem::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	OnItemMouseEnterDelegate.Broadcast(this);
}

void UObsidianItem::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	OnItemMouseLeaveDelegate.Broadcast();
}

void UObsidianItem::InitializeItemWidget(const FObsidianItemPosition& DesiredPosition, const FIntPoint& InItemGridSpan,
	UTexture2D* ItemImage, const int32 CurrentStack)
{
	ItemPosition = DesiredPosition;
	InternalStacks = CurrentStack;
	
	Root_SizeBox->SetWidthOverride(InItemGridSpan.X * ObsidianInventoryItemsStatics::InventorySlotSize.X);
	Root_SizeBox->SetHeightOverride(InItemGridSpan.Y * ObsidianInventoryItemsStatics::InventorySlotSize.Y);
	Item_Image->SetBrushFromTexture(ItemImage);
	
	if(CurrentStack == 0)
	{
		StackCount_TextBlock->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}
	
	StackCount_TextBlock->SetText(FText::AsNumber(CurrentStack));
	StackCount_TextBlock->SetVisibility(ESlateVisibility::Visible);
}

void UObsidianItem::InitializeItemWidget(const FIntPoint& InItemGridSpan, UTexture2D* ItemImage, const bool bIsForSwapSlot)
{
	const float SlotSizeMultiplier = bIsForSwapSlot == true ? SwapSlotSizeMultiplier : 1.0f;
		
	const float WidthOverride = (InItemGridSpan.X * ObsidianInventoryItemsStatics::InventorySlotSize.X) * SlotSizeMultiplier;
	const float HeightOverride = (InItemGridSpan.Y * ObsidianInventoryItemsStatics::InventorySlotSize.Y) * SlotSizeMultiplier;
	Root_SizeBox->SetWidthOverride(WidthOverride);
	Root_SizeBox->SetHeightOverride(HeightOverride);

	FSlateBrush Brush;
	Brush.SetImageSize(FVector2D(WidthOverride, HeightOverride));
	Brush.SetResourceObject(ItemImage);
	Item_Image->SetBrush(Brush);
	
	StackCount_TextBlock->SetVisibility(ESlateVisibility::Collapsed);
}

void UObsidianItem::AddCurrentStackCount(const int32 StackCountToAdd)
{
	if(StackCountToAdd <= 0)
	{
		return;
	}
	
	InternalStacks += StackCountToAdd;
	if(InternalStacks <= 0)
	{
		StackCount_TextBlock->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}
	StackCount_TextBlock->SetText(FText::AsNumber(InternalStacks));
	StackCount_TextBlock->SetVisibility(ESlateVisibility::Visible);
}

void UObsidianItem::OverrideCurrentStackCount(const int32 NewStackCount)
{
	if(NewStackCount <= 0)
	{
		return;
	}
	InternalStacks = NewStackCount;
	
	StackCount_TextBlock->SetText(FText::AsNumber(NewStackCount));
	StackCount_TextBlock->SetVisibility(ESlateVisibility::Visible);
}

FObsidianItemPosition UObsidianItem::GetItemPosition() const
{
	return ItemPosition;
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
