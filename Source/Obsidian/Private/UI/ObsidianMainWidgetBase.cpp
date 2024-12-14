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
	
	OnMouseEnterLeaveDelegate.Broadcast(false);
	UE_LOG(LogTemp, Warning, TEXT("Left Main Obsidian Widget"));
}

FReply UObsidianMainWidgetBase::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	// @HACK displaying mouse button down event fixes a bug when we click on the globe, move the mouse over and have our mouse movement blocked.
	return FReply::Handled();
}

