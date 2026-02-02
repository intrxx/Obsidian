// Copyright 2026 out of sCope team - intrxx

#include "UI/InventoryItems/Items/ObsidianItemLabel.h"

#include <CommonTextBlock.h>
#include <Components/Image.h>


void UObsidianItemLabel::NativeConstruct()
{
	Super::NativeConstruct();

	if(Background_Image)
	{
		Background_Image->SetBrushTintColor(RegularBackgroundColor);
	}
}

void UObsidianItemLabel::SetItemName(const FText& ItemName)
{
	if(ItemName_TextBlock)
	{
		ItemName_TextBlock->SetText(ItemName);
	}
}

void UObsidianItemLabel::HandleItemLabelHighlightBegin() const
{
	if(Background_Image)
	{
		Background_Image->SetBrushTintColor(HoveredBackgroundColor);
	}
}

void UObsidianItemLabel::HandleItemLabelHighlightEnd() const
{
	if(Background_Image)
	{
		Background_Image->SetBrushTintColor(RegularBackgroundColor);
	}
}

FReply UObsidianItemLabel::NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	//TODO(intrxx) Maybe some widget animation
	if(InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		const int32 Index = InMouseEvent.GetUserIndex();
		FObsidianItemInteractionFlags InteractionFlags;
		InteractionFlags.bAutomaticallyAddToWindow = InMouseEvent.IsLeftControlDown();
		
		OnItemLabelMouseButtonDownDelegate.Broadcast(Index, InteractionFlags);
	}
	return FReply::Handled();
}

void UObsidianItemLabel::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	
	HandleItemLabelHighlightBegin();
	OnItemLabelMouseHoverDelegate.Broadcast(true);
}

void UObsidianItemLabel::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	
	HandleItemLabelHighlightEnd();
	OnItemLabelMouseHoverDelegate.Broadcast(false);
}
