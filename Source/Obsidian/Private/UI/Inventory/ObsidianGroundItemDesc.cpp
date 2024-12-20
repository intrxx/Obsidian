// Copyright 2024 out of sCope team - Michał Ogiński

#include "UI/Inventory/ObsidianGroundItemDesc.h"
#include "CommonTextBlock.h"
#include "Components/Image.h"

void UObsidianGroundItemDesc::NativeConstruct()
{
	Super::NativeConstruct();

	if(Background_Image)
	{
		Background_Image->SetBrushTintColor(RegularBackgroundColor);
	}
}

void UObsidianGroundItemDesc::SetItemName(const FText& ItemName)
{
	if(ItemName_TextBlock)
	{
		ItemName_TextBlock->SetText(ItemName);
	}
}

FReply UObsidianGroundItemDesc::NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	//TODO Maybe some widget animation
	if(InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		OnItemDescMouseButtonDownDelegate.Broadcast();
	}
	return FReply::Handled();
}

void UObsidianGroundItemDesc::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	
	if(Background_Image)
	{
		Background_Image->SetBrushTintColor(HoveredBackgroundColor);
	}
	OnItemDescMouseHoverDelegate.Broadcast(true);
}

void UObsidianGroundItemDesc::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	
	if(Background_Image)
	{
		Background_Image->SetBrushTintColor(RegularBackgroundColor);
	}
	OnItemDescMouseHoverDelegate.Broadcast(false);
}
