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

FReply UObsidianItemWorldName::NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	//TODO Maybe some widget animation
	if(InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		const bool bLeftControlDown = InMouseEvent.IsLeftControlDown();
		OnItemDescMouseButtonDownDelegate.Broadcast(bLeftControlDown);
	}
	return FReply::Handled();
}

void UObsidianItemWorldName::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	
	if(Background_Image)
	{
		Background_Image->SetBrushTintColor(HoveredBackgroundColor);
	}
	OnItemDescMouseHoverDelegate.Broadcast(true);
}

void UObsidianItemWorldName::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	
	if(Background_Image)
	{
		Background_Image->SetBrushTintColor(RegularBackgroundColor);
	}
	OnItemDescMouseHoverDelegate.Broadcast(false);
}