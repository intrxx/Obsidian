// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/GameTabsMenu/ObsidianOverlayGameTabsMenu.h"
#include "Components/Button.h"
#include "UI/GameTabsMenu/Subwidgets/ObsidianGameTabButton.h"

void UObsidianOverlayGameTabsMenu::NativeConstruct()
{
	Super::NativeConstruct();

	if(CharacterStatus_GameTabButton && CharacterStatus_GameTabButton->Tab_Button)
	{
		CharacterStatus_GameTabButton->Tab_Button->OnClicked.AddDynamic(this, &ThisClass::OnCharacterStatusButtonClicked);
		
		OnCharacterStatusTabStatusChangeDelegate.AddUObject(this, &ThisClass::OnCharacterStatusTabStatusChange);
	}
}

void UObsidianOverlayGameTabsMenu::OnCharacterStatusButtonClicked()
{
	OnCharacterStatusButtonClickedDelegate.Broadcast();
}

void UObsidianOverlayGameTabsMenu::OnCharacterStatusTabStatusChange(bool bIsConstructed)
{
	if(CharacterStatus_GameTabButton)
	{
		CharacterStatus_GameTabButton->bIsCorrespondingTabOpen = bIsConstructed;
	}
}
