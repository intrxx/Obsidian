// Copyright 2026 out of sCope team - intrxx

#include "UI/Settings/ObsidianSettingsMenu.h"

// ~ Core
#include "Input/CommonUIInputTypes.h"

// ~ Project

void UObsidianSettingsMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	BackwardsHandle = RegisterUIActionBinding(FBindUIActionArgs(BackwardsInputActionData, false, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleBackwardsAction)));
}

void UObsidianSettingsMenu::HandleBackwardsAction()
{
	//TODO(intrxx) Apply any changes
	
	DeactivateWidget();
}
