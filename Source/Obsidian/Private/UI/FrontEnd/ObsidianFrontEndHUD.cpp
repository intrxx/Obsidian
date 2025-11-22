// Copyright 2024 out of sCope team - Michał Ogiński

#include "UI/FrontEnd/ObsidianFrontEndHUD.h"

#include "UI/WidgetControllers/ObCharacterSelectionWidgetController.h"

AObsidianFrontEndHUD::AObsidianFrontEndHUD(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UObCharacterSelectionWidgetController* AObsidianFrontEndHUD::GetCharacterSelectionWidgetController(
	const FObsidianWidgetControllerParams& WidgetControllerParams)
{
	if(CharacterSelectionWidgetController == nullptr)
	{
		if(ensureMsgf(CharacterSelectionWidgetControllerClass, TEXT("Inventory Controller Class is not set on HUD Class [%s], please fill it out in BP_ObsidianHUD"), *GetNameSafe(this)))
		{
			CharacterSelectionWidgetController = NewObject<UObCharacterSelectionWidgetController>(this, CharacterSelectionWidgetControllerClass);
			CharacterSelectionWidgetController->SetWidgetControllerParams(WidgetControllerParams);
			CharacterSelectionWidgetController->OnWidgetControllerSetupCompleted();
			
			return CharacterSelectionWidgetController;
		}
	}
	return CharacterSelectionWidgetController;
}

