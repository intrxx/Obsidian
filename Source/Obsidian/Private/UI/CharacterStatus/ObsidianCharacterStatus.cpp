// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/CharacterStatus/ObsidianCharacterStatus.h"
#include "Components/ScrollBox.h"

void UObsidianCharacterStatus::NativeConstruct()
{
	Super::NativeConstruct();

	CurrentlyShownTab = Offence_ScrollBox;
}

void UObsidianCharacterStatus::SwitchToTab(UScrollBox* Tab)
{
	if(Tab == nullptr)
	{
		return;
	}

	CurrentlyShownTab->SetVisibility(ESlateVisibility::Collapsed);
	Tab->SetVisibility(ESlateVisibility::Visible);
	CurrentlyShownTab = Tab;
}
