// Copyright 2024 out of sCope team - Michał Ogiński

#include "UI/WidgetControllers/ObCharacterSelectionWidgetController.h"


void UObCharacterSelectionWidgetController::OnWidgetControllerSetupCompleted()
{
	check(ObsidianLocalPlayer);
	check(ObsidianPlayerController);

	UE_LOG(LogTemp, Display, TEXT("Character Selection Widget Controller has completed the setup! Hi [%s]"), *ObsidianLocalPlayer.GetName());
}
