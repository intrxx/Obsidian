// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/ObsidianWidgetBase.h"

void UObsidianWidgetBase::SetWidgetController(UObject* InWidgetController)
{
	WidgetController = InWidgetController;
	Blueprint_HandleWidgetControllerSet();
	HandleWidgetControllerSet();
}

void UObsidianWidgetBase::HandleWidgetControllerSet()
{
}
