// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/ObsidianWidgetController.h"

void UObsidianWidgetController::SetWidgetControllerParams(const FWidgetControllerParams& WidgetControllerParams)
{
	PlayerController = WidgetControllerParams.PlayerController;
	PlayerState = WidgetControllerParams.PlayerState;
	AbilitySystemComponent = WidgetControllerParams.AbilitySystemComponent;
	AttributesComponent = WidgetControllerParams.AttributesComponent;
}

void UObsidianWidgetController::OnWidgetControllerSetupCompleted()
{
}
