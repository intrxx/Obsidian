// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/ObsidianWidgetBase.h"

void UObsidianWidgetBase::SetWidgetController(UObject* InWidgetController)
{
	WidgetController = InWidgetController;
	HandleWidgetControllerSet();
	BP_HandleWidgetControllerSet();

	PostHandleWidgetControllerSet();
}

void UObsidianWidgetBase::HandleWidgetControllerSet()
{
}

void UObsidianWidgetBase::PostHandleWidgetControllerSet()
{
}

