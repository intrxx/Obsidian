// Copyright 2026 out of sCope team - intrxx

#include "UI/ObsidianWidgetBase.h"

// ~ Core

// ~ Project

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

