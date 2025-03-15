// Copyright 2024 out of sCope team - Michał Ogiński

#include "UI/ObsidianMainWidgetBase.h"

void UObsidianMainWidgetBase::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	
	OnMouseEnterLeaveDelegate.Broadcast(true);
}

void UObsidianMainWidgetBase::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	
	OnMouseEnterLeaveDelegate.Broadcast(false);
}

FReply UObsidianMainWidgetBase::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	// @HACK displaying mouse button down event fixes a bug when we click on the globe, move the mouse over and have our mouse movement blocked.
	if(InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		return FReply::Handled();
	}
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

