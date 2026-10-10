// Copyright 2026 out of sCope team - intrxx

#include "UI/FrontEnd/ObsidianFrontEndHUD.h"

#include "UI/WidgetControllers/ObCharacterSelectionWidgetController.h"


AObsidianFrontEndHUD::AObsidianFrontEndHUD(const FObjectInitializer& InObjectInitializer)
	: Super(InObjectInitializer)
{
}

UObCharacterSelectionWidgetController* AObsidianFrontEndHUD::GetCharacterSelectionWidgetController(
	const FObsidianWidgetControllerParams& InWidgetControllerParams)
{
	if(CharacterSelectionWidgetController == nullptr)
	{
		if(ensureMsgf(CharacterSelectionWidgetControllerClass, TEXT("Inventory Controller Class is not set on HUD Class [%s], please fill it out in BP_ObsidianHUD"), *GetNameSafe(this)))
		{
			CharacterSelectionWidgetController = NewObject<UObCharacterSelectionWidgetController>(this, CharacterSelectionWidgetControllerClass);
			CharacterSelectionWidgetController->SetWidgetControllerParams(InWidgetControllerParams);
			CharacterSelectionWidgetController->OnWidgetControllerSetupCompleted();
			
			return CharacterSelectionWidgetController;
		}
	}
	return CharacterSelectionWidgetController;
}

