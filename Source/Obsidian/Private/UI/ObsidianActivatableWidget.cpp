// Copyright 2026 out of sCope team - intrxx

#include "UI/ObsidianActivatableWidget.h"

#include "ICommonInputModule.h"


#define LOCTEXT_NAMESPACE "Obsidian"

UObsidianActivatableWidget::UObsidianActivatableWidget(const FObjectInitializer& InObjectInitializer)
	: Super(InObjectInitializer)
{
}

void UObsidianActivatableWidget::SetWidgetController(UObject* InWidgetController)
{
	WidgetController = InWidgetController;
	HandleWidgetControllerSet();
	BP_HandleWidgetControllerSet();

	PostHandleWidgetControllerSet();
}

void UObsidianActivatableWidget::HandleWidgetControllerSet()
{
}

void UObsidianActivatableWidget::PostHandleWidgetControllerSet()
{
}

TOptional<FUIInputConfig> UObsidianActivatableWidget::GetDesiredInputConfig() const
{
	switch(InputConfig)
	{
	case EObsidianWidgetInputMode::GameAndMenu:
		{
			return FUIInputConfig(ECommonInputMode::All, GameMouseCaptureMode);
		}
	case EObsidianWidgetInputMode::Game:
		{
			return FUIInputConfig(ECommonInputMode::Game, GameMouseCaptureMode);
		}
	case EObsidianWidgetInputMode::Menu:
		{
			return FUIInputConfig(ECommonInputMode::Menu, GameMouseCaptureMode);
		}
	case EObsidianWidgetInputMode::Default:
	default:
		{
			return TOptional<FUIInputConfig>(); 
		}
	}
}

#if WITH_EDITOR

#include "Editor/WidgetCompilerLog.h"

void UObsidianActivatableWidget::ValidateCompiledWidgetTree(const UWidgetTree& InBlueprintWidgetTree, class IWidgetCompilerLog& InCompileLog) const
{
	Super::ValidateCompiledWidgetTree(InBlueprintWidgetTree, InCompileLog);

	if (!GetClass()->IsFunctionImplementedInScript(GET_FUNCTION_NAME_CHECKED(UObsidianActivatableWidget, BP_GetDesiredFocusTarget)))
	{
		if (GetParentNativeClass(GetClass()) == UObsidianActivatableWidget::StaticClass())
		{
			InCompileLog.Warning(LOCTEXT("ValidateGetDesiredFocusTarget_Warning", "GetDesiredFocusTarget wasn't implemented, you're going to have trouble using gamepads on this screen."));
		}
		else
		{
			//TODO - Note for now, because we can't guarantee it isn't implemented in a native subclass of this one.
			InCompileLog.Note(LOCTEXT("ValidateGetDesiredFocusTarget_Note", "GetDesiredFocusTarget wasn't implemented, you're going to have trouble using gamepads on this screen.  If it was implemented in the native base class you can ignore this message."));
		}
	}
}

#endif

#undef LOCTEXT_NAMESPACE