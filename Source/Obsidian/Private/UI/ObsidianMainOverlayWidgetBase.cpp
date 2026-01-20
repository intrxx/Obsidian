// Copyright 2026 out of sCope team - intrxx

#include "UI/ObsidianMainOverlayWidgetBase.h"

#include <Components/Button.h>
#include <Components/SizeBox.h>


void UObsidianMainOverlayWidgetBase::NativePreConstruct()
{
	Super::NativePreConstruct();

	if(bOverride_RootSizeHeight)
	{
		Root_SizeBox->SetHeightOverride(RootSizeBoxHeight);
	}
	if(bOverride_RootSizeWidth)
	{
		Root_SizeBox->SetWidthOverride(RootSizeBoxWidth);
	}
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

void UObsidianMainOverlayWidgetBase::PreCloseButtonPressed()
{
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
	PreCloseButtonPressed();
	
	RemoveFromParent();
}
