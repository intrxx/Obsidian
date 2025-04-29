// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/Settings/ObsidianSettingsMenu.h"

#include "Input/CommonUIInputTypes.h"

void UObsidianSettingsMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	BackwardsHandle = RegisterUIActionBinding(FBindUIActionArgs(BackwardsInputActionData, false, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleBackwardsAction)));
}

void UObsidianSettingsMenu::HandleBackwardsAction()
{
	//TODO Apply any changes
	
	DeactivateWidget();
}
