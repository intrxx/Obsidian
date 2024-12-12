// Copyright 2024 out of sCope team - Michał Ogiński

#include "UI/ObsidianMainWidgetBase.h"

void UObsidianMainWidgetBase::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	
	OnMouseEnterLeaveDelegate.Broadcast(true);
	UE_LOG(LogTemp, Warning, TEXT("Entered Main Obsidian Widget"));
}

void UObsidianMainWidgetBase::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

	// @Hack
	// Problem that it solves: When we click the mouse button this functions gets called, so we leave the widget and character moves.
	// Why is it questionable? I don't know if this is reliable
	if(InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		return;
	}
	
	OnMouseEnterLeaveDelegate.Broadcast(false);
	UE_LOG(LogTemp, Warning, TEXT("Left Main Obsidian Widget"));
}
