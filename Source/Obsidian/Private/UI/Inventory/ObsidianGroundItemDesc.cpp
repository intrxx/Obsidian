// Copyright 2024 out of sCope team - Michał Ogiński

#include "UI/Inventory/ObsidianGroundItemDesc.h"
#include "Components/Image.h"


void UObsidianGroundItemDesc::NativeConstruct()
{
	Super::NativeConstruct();

	if(Background_Image)
	{
		Background_Image->SetBrushTintColor(RegularBackgroundColor);		
	}
}

FReply UObsidianGroundItemDesc::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	//TODO Maybe some widget animation
	
	if(InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		OnItemDescMouseButtonDownDelegate.Broadcast();
	}
	
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UObsidianGroundItemDesc::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if(Background_Image)
	{
		Background_Image->SetBrushTintColor(HoveredBackgroundColor);
	}
	OnItemDescMouseHoverDelegate.Broadcast(true);
	
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
}

void UObsidianGroundItemDesc::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	if(Background_Image)
	{
		Background_Image->SetBrushTintColor(RegularBackgroundColor);
	}
	OnItemDescMouseHoverDelegate.Broadcast(false);
	
	Super::NativeOnMouseLeave(InMouseEvent);
}
