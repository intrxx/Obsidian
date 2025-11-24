// Copyright 2024 out of sCope team - Michał Ogiński

#include "UI/FrontEnd/ObsidianCharacterScreen.h"

#include <CommonHierarchicalScrollBox.h>
#include <CommonTextBlock.h>
#include <CommonUIExtensions.h>
#include <Kismet/GameplayStatics.h>

#include "UI/FrontEnd/ObsidianCharacterEntry.h"
#include "UI/FrontEnd/ObsidianCharacterCreationScreen.h"
#include "UI/WidgetControllers/ObCharacterSelectionWidgetController.h"
#include "Core/ObsidianGameplayStatics.h"
#include "Game/ObsidianFrontEndGameMode.h"
#include "Game/Save/ObsidianHeroSaveGame.h"
#include "Game/Save/ObsidianSaveGameSubsystem.h"
#include "Obsidian/ObsidianGameplayTags.h"
#include "UI/Components/ObsidianButtonBase.h"

void UObsidianCharacterScreen::HandleWidgetControllerSet()
{
	CharacterSelectionWidgetController = Cast<UObCharacterSelectionWidgetController>(WidgetController);
	check(CharacterSelectionWidgetController);
}

void UObsidianCharacterScreen::NativeConstruct()
{
	Super::NativeConstruct();

	Play_Button->OnClicked().AddUObject(this, &ThisClass::OnPlayClicked);
	Delete_Button->OnClicked().AddUObject(this, &ThisClass::OnDeleteClicked);
	Create_Button->OnClicked().AddUObject(this, &ThisClass::OnCreateClicked);
	
	FrontEndGameMode = Cast<AObsidianFrontEndGameMode>(UGameplayStatics::GetGameMode(this));
	ensureMsgf(FrontEndGameMode, TEXT("FrontEndGameMode is invalid in UObsidianCharacterCreationScreen::NativeOnActivated()"));
}

void UObsidianCharacterScreen::NativeDestruct()
{
	if (Play_Button && Delete_Button && Create_Button)
	{
		Play_Button->OnClicked().Clear();
		Delete_Button->OnClicked().Clear();
		Create_Button->OnClicked().Clear();
	}
	
	Super::NativeDestruct();
}

void UObsidianCharacterScreen::NativeOnActivated()
{
	Super::NativeOnActivated();
	
	PopulateCharacterScreen();
}

void UObsidianCharacterScreen::NativeOnDeactivated()
{
	if(FrontEndGameMode && CachedChosenCharacterEntry)
	{
		CachedChosenCharacterEntry->ResetChosenState();
		CachedChosenCharacterEntry = nullptr;
	}
	
	Super::NativeOnDeactivated();
}

UWidget* UObsidianCharacterScreen::NativeGetDesiredFocusTarget() const
{
	if(CharacterEntries.IsEmpty())
	{
		return Create_Button;
	}
	return Play_Button;
}

void UObsidianCharacterScreen::InitializeOnlineCharacterScreen()
{
	TabName_TextBlock->SetText(FText::FromString(TEXT("Online Character Screen")));
	bIsOnline = true;
}

void UObsidianCharacterScreen::InitializeOfflineCharacterScreen()
{
	TabName_TextBlock->SetText(FText::FromString(TEXT("Character Screen")));
	bIsOnline = false;
}

void UObsidianCharacterScreen::OnPlayClicked()
{
	if (bIsOnline)
	{
		UCommonUIExtensions::PushStreamedContentToLayer_ForPlayer(GetOwningLocalPlayer(), ObsidianGameplayTags::UI_Layer_MainMenu,
			SoftOnlineLobbyWidgetClass);
		return;
	}

	UGameplayStatics::OpenLevel(GetWorld(), FName("L_Tutorial_01"));
}

void UObsidianCharacterScreen::OnDeleteClicked()
{
	if(CachedChosenCharacterEntry && FrontEndGameMode && CharacterEntries.Contains(CachedChosenCharacterEntry))
	{
		CharacterEntries.Remove(CachedChosenCharacterEntry);
		CachedChosenCharacterEntry->RemoveFromParent();
		FrontEndGameMode->DeleteHeroClass(CachedChosenCharacterEntry->TempSaveID);

		if(CharacterEntries.IsEmpty() == false)
		{
			if(UObsidianCharacterEntry* NewEntry = CharacterEntries[0])
			{
				NewEntry->SetIsChosen();
				CachedChosenCharacterEntry = NewEntry;
			}
		}
		else
		{
			Play_Button->SetIsEnabled(false);
		}
	}
}

void UObsidianCharacterScreen::OnCreateClicked()
{
	if (LoadedCharacterCreationScreenClass == nullptr)
	{
		LoadedCharacterCreationScreenClass = SoftCharacterCreationScreenClass.LoadSynchronous();
		UE_LOG(LogTemp, Warning, TEXT("Needed to load CharacterSelectionWidgetClass Synchronously!"));
	}
	
	UObsidianCharacterCreationScreen* CharacterCreationScreen = Cast<UObsidianCharacterCreationScreen>(
		UCommonUIExtensions::PushContentToLayer_ForPlayer(GetOwningLocalPlayer(),
			ObsidianGameplayTags::UI_Layer_MainMenu,
			LoadedCharacterCreationScreenClass));

	if (CharacterCreationScreen)
	{
		CharacterCreationScreen->InitializeCharacterCreationScreen(bIsOnline);
		CharacterCreationScreen->SetWidgetController(CharacterSelectionWidgetController);
		CharacterSelectionWidgetController->SetupCameraForCreationPanel();
	}
}

void UObsidianCharacterScreen::PopulateCharacterScreen()
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	if(PlayerController == nullptr)
	{
		return;
	}

	if(CharacterEntries.IsEmpty() == false)
	{
		for(UObsidianCharacterEntry* Entry : CharacterEntries)
		{
			Entry->RemoveFromParent();
		}
		CharacterEntries.Reset();
	}
	
	if(bIsOnline == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("Populating Offline Characters"));
		
		if (const UGameInstance* GameInstance = GetGameInstance())
		{
			if (UObsidianSaveGameSubsystem* SaveGameSubsystem = GameInstance->GetSubsystem<UObsidianSaveGameSubsystem>())
			{
				if (OnPopulateLoadingFinishedDelegateHandle.IsValid())
				{
					SaveGameSubsystem->OnLoadingFinishedDelegate.Remove(OnPopulateLoadingFinishedDelegateHandle);
				}

				OnPopulateLoadingFinishedDelegateHandle = SaveGameSubsystem->OnLoadingFinishedDelegate.AddUObject(this,
						&ThisClass::OnPopulateCharacterLoadingFinished);
				SaveGameSubsystem->RequestLoadGame(true, GetOwningLocalPlayer<UObsidianLocalPlayer>());
			}	
		}
		return;
	}
}

void UObsidianCharacterScreen::InitCharacterScreen()
{
	for(UObsidianCharacterEntry* Entry : CharacterEntries)
	{
		if(Entry && Entry->OnClicked().IsBound() == false)
		{
			Entry->OnEntryClicked.AddUObject(this, &ThisClass::HandleClickingOnCharacterEntry);
		}
	}

	if(CharacterEntries.IsEmpty() == false)
	{
		CachedChosenCharacterEntry = CharacterEntries[0];
		CachedChosenCharacterEntry->SetIsChosen();

		if(Play_Button->GetIsEnabled() == false)
		{
			Play_Button->SetIsEnabled(true);
		}
		
		if(UCommonUIExtensions::IsOwningPlayerUsingGamepad(this))
		{
			Play_Button->SetFocus();
		}
	}
	else
	{
		Play_Button->SetIsEnabled(false);
	}
}

void UObsidianCharacterScreen::HandleClickingOnCharacterEntry(UObsidianCharacterEntry* EntryClicked)
{
	if(EntryClicked == nullptr)
	{
		return;
	}

	if(CachedChosenCharacterEntry == EntryClicked)
	{
		return;
	}

	if(CachedChosenCharacterEntry)
	{
		CachedChosenCharacterEntry->ResetChosenState();
	}

	EntryClicked->SetIsChosen();
	CachedChosenCharacterEntry = EntryClicked;
}

void UObsidianCharacterScreen::OnPopulateCharacterLoadingFinished(UObsidianHeroSaveGame* SaveGame, bool bSuccess)
{
	if (const UGameInstance* GameInstance = GetGameInstance())
	{
		if (UObsidianSaveGameSubsystem* SaveGameSubsystem = GameInstance->GetSubsystem<UObsidianSaveGameSubsystem>())
		{
			SaveGameSubsystem->OnLoadingFinishedDelegate.Remove(OnPopulateLoadingFinishedDelegateHandle);
		}
	}
	
	if (bSuccess && SaveGame)
	{
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
		if(PlayerController == nullptr)
		{
			return;
		}
		
		const FObsidianHeroSaveData HeroSaveData = SaveGame->GetHeroSaveData();
		
		checkf(CharacterEntryWidgetClass, TEXT("CharacterEntryWidgetClass is invalid in UObsidianCharacterScreen::PopulateCharacterScreen."));
		UObsidianCharacterEntry* Entry = CreateWidget<UObsidianCharacterEntry>(PlayerController, CharacterEntryWidgetClass);
		Entry->InitializeCharacterEntry(HeroSaveData.InitializationSaveData.PlayerHeroName, HeroSaveData.GameplaySaveData.HeroLevel,
			UObsidianGameplayStatics::GetHeroClassText(HeroSaveData.InitializationSaveData.HeroClass),
			false, HeroSaveData.InitializationSaveData.bHardcore);
		Entry->TempSaveID = HeroSaveData.InitializationSaveData.HeroID;
							
		CharacterList_ScrollBox->AddChild(Entry);
		CharacterEntries.Add(Entry);
	}
					
	InitCharacterScreen();
}
