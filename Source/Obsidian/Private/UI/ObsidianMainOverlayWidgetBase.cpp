// Copyright 2024 out of sCope team - Michał Ogiński

#include "UI/ObsidianMainOverlayWidgetBase.h"

// ~ Core
#include "Components/Button.h"

// ~ Project

void UObsidianMainOverlayWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();

	Close_Button->OnClicked.AddDynamic(this, &ThisClass::OnCloseButtonClicked);
}

void UObsidianMainOverlayWidgetBase::NativeDestruct()
{
	OnWidgetDestroyedDelegate.Broadcast();
	Close_Button->OnClicked.Clear();
	
	Super::NativeDestruct();
}

void UObsidianMainOverlayWidgetBase::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
}

void UObsidianMainOverlayWidgetBase::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
}

FReply UObsidianMainOverlayWidgetBase::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	// This widgets won't take any input, don't want to pass gameplay input through
	return FReply::Handled();
}

FReply UObsidianMainOverlayWidgetBase::NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	// This widgets won't take any input, don't want to pass gameplay input through
	return FReply::Handled();
}

void UObsidianMainOverlayWidgetBase::OnCloseButtonClicked()
{
	RemoveFromParent();
}
