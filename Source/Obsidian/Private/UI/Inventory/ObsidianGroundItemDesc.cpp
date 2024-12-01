// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/Inventory/ObsidianGroundItemDesc.h"

FReply UObsidianGroundItemDesc::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if(InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		OnItemDescMouseButtonDownDelegate.Broadcast();
	}
	
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UObsidianGroundItemDesc::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	OnItemDescMouseHoverDelegate.Broadcast(true);
	
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
}

void UObsidianGroundItemDesc::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	OnItemDescMouseHoverDelegate.Broadcast(false);
	
	Super::NativeOnMouseLeave(InMouseEvent);
}
