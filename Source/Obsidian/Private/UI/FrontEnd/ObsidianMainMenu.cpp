// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/FrontEnd/ObsidianMainMenu.h"

#include "CommonButtonBase.h"

void UObsidianMainMenu::NativeConstruct()
{
	Super::NativeConstruct();

	if(Play_Button && Options_Button && Exit_Button)
	{
		Play_Button->OnClicked.AddDynamic(this, &ThisClass::OnPlayButtonClicked);
		Options_Button->OnClicked.AddDynamic(this, &ThisClass::OnOptionsButtonClicked);
		Exit_Button->OnClicked.AddDynamic(this, &ThisClass::OnExitButtonClicked);
	}
}

void UObsidianMainMenu::OnPlayButtonClicked()
{
}

void UObsidianMainMenu::OnOptionsButtonClicked()
{
	
}

void UObsidianMainMenu::OnExitButtonClicked()
{
	FGenericPlatformMisc::RequestExit(false);
}
