// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/ObsidianCharacterStatus.h"
#include "Components/ScrollBox.h"

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
