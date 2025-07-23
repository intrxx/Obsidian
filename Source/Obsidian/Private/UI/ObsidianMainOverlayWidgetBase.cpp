// Copyright 2024 out of sCope team - Michał Ogiński

#include "UI/ObsidianMainOverlayWidgetBase.h"

// ~ Core
#include "Components/Button.h"
#include "Components/SizeBox.h"

// ~ Project

void UObsidianMainOverlayWidgetBase::NativePreConstruct()
{
	Super::NativePreConstruct();

	Root_SizeBox->SetHeightOverride(RootSizeBoxHeight);
	Root_SizeBox->SetWidthOverride(RootSizeBoxWidth);
}

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
