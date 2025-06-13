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

FReply UObsidianMainOverlayWidgetBase::NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
#if !UE_BUILD_SHIPPING
	UE_LOG(LogTemp, Display, TEXT("The double click was handled in UObsidianMainOverlayWidgetBase::NativeOnMouseButtonDoubleClick to prevent moving with mouse, if that not what you want to do, please reconsider this logic."));
#endif 
	return FReply::Handled();
}

void UObsidianMainOverlayWidgetBase::OnCloseButtonClicked()
{
	OnMouseEnterLeaveDelegate.Broadcast(false);
	RemoveFromParent();
}
