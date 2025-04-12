// Copyright 2024 out of sCope team - Michał Ogiński

#include "Game/ObsidianFrontEndGameMode.h"
#include "CommonUIExtensions.h"
#include "Obsidian/ObsidianGameplayTags.h"

void AObsidianFrontEndGameMode::BeginPlay()
{
	Super::BeginPlay();

	if(UGameInstance* GameInstance = GetGameInstance())
	{
		GameInstance->OnLocalPlayerAddedEvent.AddUObject(this, &ThisClass::OnLocalPlayerAdded);	
	}
}

void AObsidianFrontEndGameMode::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);
	
	TryToShowMainMenu();
}

void AObsidianFrontEndGameMode::TryToShowMainMenu()
{
	// checkf(MainMenuWidgetClass.IsNull() == false, TEXT("SettingsMenuWidgetClass is invalid in AObsidianFrontEndGameMode::TryToShowMainMenu(), please fill it on ObsidianFrontEndGameMode."));
	//
	// APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	// UAsyncAction_PushContentToLayerForPlayer::PushContentToLayerForPlayer(PC, MainMenuWidgetClass, ObsidianGameplayTags::UI_Layer_MainMenu, true);
}

void AObsidianFrontEndGameMode::OnLocalPlayerAdded(ULocalPlayer* NewPlayer)
{
	UE_LOG(LogTemp, Error, TEXT("Local Player Added"));
	
	if(UWorld* World = GetWorld())
	{
		UCommonUIExtensions::PushContentToLayer_ForPlayer(NewPlayer, ObsidianGameplayTags::UI_Layer_MainMenu, MainMenuWidgetClass);
	}
}
	
