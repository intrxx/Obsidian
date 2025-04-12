// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/FrontEnd/ObsidianMainMenu.h"
#include "CommonButtonBase.h"
#include "UI/Settings/ObsidianSettingsMenu.h"
#include "Actions/AsyncAction_PushContentToLayerForPlayer.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Obsidian/ObsidianGameplayTags.h"

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
	UE_LOG(LogTemp, Display, TEXT("Options Play Clicked."));
	//UAsyncAction_PushContentToLayerForPlayer::PushContentToLayerForPlayer(GetOwningPlayer(), )
}

void UObsidianMainMenu::OnOptionsButtonClicked()
{
	UE_LOG(LogTemp, Display, TEXT("Options Button Clicked."));
	
	checkf(SettingsMenuWidgetClass.IsNull() == false, TEXT("SettingsMenuWidgetClass is invalid in UObsidianMainMenu::OnOptionsButtonClicked, please fill it on ObsidianMainMenu."));
	UAsyncAction_PushContentToLayerForPlayer::PushContentToLayerForPlayer(GetOwningPlayer(), SettingsMenuWidgetClass, ObsidianGameplayTags::UI_Layer_MainMenu);
}

void UObsidianMainMenu::OnExitButtonClicked()
{
	UE_LOG(LogTemp, Display, TEXT("Exit Button Clicked."));
	
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, false);
}
