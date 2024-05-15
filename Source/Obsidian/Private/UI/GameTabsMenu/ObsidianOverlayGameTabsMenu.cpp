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
		
		OnCharacterStatusTabStatusChangeDelegate.AddLambda([this](bool bIsConstructed)
		{
			CharacterStatus_GameTabButton->bIsCorrespondingTabOpen = bIsConstructed;
		});
	}
}

void UObsidianOverlayGameTabsMenu::OnCharacterStatusButtonClicked()
{
	OnCharacterStatusButtonClickedDelegate.Broadcast();
}
