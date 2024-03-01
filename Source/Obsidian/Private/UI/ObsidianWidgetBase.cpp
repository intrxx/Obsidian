// Copyright 2024 Michał Ogiński


#include "UI/ObsidianWidgetBase.h"

void UObsidianWidgetBase::SetWidgetController(UObject* InWidgetController)
{
	WidgetController = InWidgetController;
	HandleWidgetControllerSet();
}