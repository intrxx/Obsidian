// Copyright 2024 out of sCope team - Michał Ogiński

#include "UI/GameplayMenu/ObsidianGameplayMenu.h"

#include <CommonUIExtensions.h>

#include "Game/Save/ObsidianSaveGameSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Obsidian/ObsidianGameplayTags.h"
#include "UI/Components/ObsidianButtonBase.h"
#include "Characters/Player/ObsidianLocalPlayer.h"

void UObsidianGameplayMenu::NativeConstruct()
{
	Super::NativeConstruct();

	if (Continue_Button && Options_Button && SaveAndExit_Button)
	{
		Continue_Button->OnClicked().AddUObject(this, &ThisClass::OnContinueClicked);
		Options_Button->OnClicked().AddUObject(this, &ThisClass::OnOptionsClicked);
		SaveAndExit_Button->OnClicked().AddUObject(this, &ThisClass::OnSaveAndExitClicked);
	}
}

void UObsidianGameplayMenu::NativeDestruct()
{
	if (Continue_Button && Options_Button && SaveAndExit_Button)
	{
		Continue_Button->OnClicked().Clear();
		Options_Button->OnClicked().Clear();
		SaveAndExit_Button->OnClicked().Clear();
	}
	
	Super::NativeDestruct();
}

UWidget* UObsidianGameplayMenu::NativeGetDesiredFocusTarget() const
{
	return Continue_Button;
}

void UObsidianGameplayMenu::OnContinueClicked()
{
	UCommonUIExtensions::PopContentFromLayer(this);
}

void UObsidianGameplayMenu::OnOptionsClicked()
{
	ULocalPlayer* LocalPlayer = GetOwningLocalPlayer();
	if (SettingsMenuWidgetClass.IsNull() == false && LocalPlayer)
	{
		UCommonUIExtensions::PushStreamedContentToLayer_ForPlayer(LocalPlayer, ObsidianGameplayTags::UI_Layer_GameplayMenu,
			SettingsMenuWidgetClass);
	}
}

void UObsidianGameplayMenu::OnSaveAndExitClicked()
{
	if (const UGameInstance* GameInstance = GetGameInstance())
	{
		if (UObsidianSaveGameSubsystem* SaveGameSubsystem = GameInstance->GetSubsystem<UObsidianSaveGameSubsystem>())
		{
			OnSaveFinishedDelegateHandle = SaveGameSubsystem->OnSavingFinishedDelegate.AddUObject(this,
				&ThisClass::OnSaveFinished);
			SaveGameSubsystem->RequestSaveGame(true, GetOwningLocalPlayer<UObsidianLocalPlayer>());
		}
	}
}

void UObsidianGameplayMenu::OnSaveFinished(UObsidianSaveGame* SaveGame, bool bSuccess)
{
	if (const UGameInstance* GameInstance = GetGameInstance())
	{
		if (UObsidianSaveGameSubsystem* SaveGameSubsystem = GameInstance->GetSubsystem<UObsidianSaveGameSubsystem>())
		{
			SaveGameSubsystem->OnSavingFinishedDelegate.Remove(OnSaveFinishedDelegateHandle);
		}
	}

	const UWorld* World = GetWorld();
	if (bSuccess && World)
	{
		UGameplayStatics::OpenLevel(World, FName("L_FrontEnd"), true);
	}
}
