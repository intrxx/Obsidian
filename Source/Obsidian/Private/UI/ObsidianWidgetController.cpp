// Copyright 2024 Michał Ogiński


#include "UI/ObsidianWidgetController.h"

void UObsidianWidgetController::SetWidgetControllerParams(const FWidgetControllerParams& WidgetControllerParams)
{
	PlayerController = WidgetControllerParams.PlayerController;
	PlayerState = WidgetControllerParams.PlayerState;
	AbilitySystemComponent = WidgetControllerParams.AbilitySystemComponent;
}

void UObsidianWidgetController::BroadcastControllerToAttributesComp(UObsidianAttributesComponent* AC)
{
}
