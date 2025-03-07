// Copyright 2024 out of sCope team - Michał Ogiński

#include "UI/Inventory/ObsidianItemWorldName.h"
#include "CommonTextBlock.h"
#include "Components/Image.h"

void UObsidianItemWorldName::NativeConstruct()
{
	Super::NativeConstruct();

	if(Background_Image)
	{
		Background_Image->SetBrushTintColor(RegularBackgroundColor);
	}
}

void UObsidianItemWorldName::SetItemName(const FText& ItemName)
{
	if(ItemName_TextBlock)
	{
		ItemName_TextBlock->SetText(ItemName);
	}
}

void UObsidianItemWorldName::HandleWorldNameHighlightBegin() const
{
	if(Background_Image)
	{
		Background_Image->SetBrushTintColor(HoveredBackgroundColor);
	}
}

void UObsidianItemWorldName::HandleWorldNameHighlightEnd() const
{
	if(Background_Image)
	{
		Background_Image->SetBrushTintColor(RegularBackgroundColor);
	}
}

FReply UObsidianItemWorldName::NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	//TODO Maybe some widget animation
	if(InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		const int32 Index = InMouseEvent.GetUserIndex();
		const bool bLeftControlDown = InMouseEvent.IsLeftControlDown();
		OnItemWorldNameMouseButtonDownDelegate.Broadcast(Index, bLeftControlDown);
	}
	return FReply::Handled();
}

void UObsidianItemWorldName::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	
	HandleWorldNameHighlightBegin();
	OnItemWorldNameMouseHoverDelegate.Broadcast(true);
}

void UObsidianItemWorldName::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	
	HandleWorldNameHighlightEnd();
	OnItemWorldNameMouseHoverDelegate.Broadcast(false);
}
