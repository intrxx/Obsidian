// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/CharacterStatus/ObsidianCharacterStatus.h"

#include "Components/Button.h"
#include "Components/ScrollBox.h"

void UObsidianCharacterStatus::NativeConstruct()
{
	Super::NativeConstruct();

	CurrentlyShownTab = Offence_ScrollBox;

	Close_Button->OnClicked.AddDynamic(this, &ThisClass::OnCloseButtonClicked);
}

void UObsidianCharacterStatus::NativeDestruct()
{
	OnCharacterStatusDestroyedDelegate.Broadcast();
	
	Super::NativeDestruct();
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

void UObsidianCharacterStatus::OnCloseButtonClicked()
{
	RemoveFromParent();
}
