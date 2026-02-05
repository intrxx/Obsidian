// Copyright 2026 out of sCope team - intrxx

#include "UI/InventoryItems/Items/ObsidianItemLabel.h"

#include <CommonTextBlock.h>
#include <Components/Border.h>

UObsidianItemLabel::UObsidianItemLabel()
{
	RegularBrushColor = FLinearColor(1.0f, 1.0f, 1.0f, 0.8f);
	HoveredBrushColor = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

void UObsidianItemLabel::NativeConstruct()
{
	Super::NativeConstruct();

	if(Main_Border)
	{
		Main_Border->SetBrushColor(RegularBrushColor);
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
	if(Main_Border)
	{
		Main_Border->SetBrushColor(HoveredBrushColor);
	}
}

void UObsidianItemLabel::HandleItemLabelHighlightEnd() const
{
	if(Main_Border)
	{
		Main_Border->SetBrushColor(RegularBrushColor);
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
