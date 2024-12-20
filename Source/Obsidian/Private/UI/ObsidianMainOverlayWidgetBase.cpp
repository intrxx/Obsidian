// Copyright 2024 out of sCope team - Michał Ogiński

#include "UI/ObsidianMainOverlayWidgetBase.h"
#include "Components/Button.h"

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

void UObsidianMainOverlayWidgetBase::OnCloseButtonClicked()
{
	OnMouseEnterLeaveDelegate.Broadcast(false);
	RemoveFromParent();
}
