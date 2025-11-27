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
#include "Game/Save/ObsidianMasterSaveGame.h"
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
	
	FrontEndGameMode = Cast<AObsidianFrontEndGameMode>(UGameplayStatics::GetGameMode(this));
	ensureMsgf(FrontEndGameMode, TEXT("FrontEndGameMode is invalid in UObsidianCharacterCreationScreen::NativeOnActivated()"));
}

void UObsidianCharacterScreen::NativeDestruct()
{
	if (Play_Button && Delete_Button && Create_Button)
	{
		Play_Button->OnClicked().Clear();
		Delete_Button->OnClicked().Clear();
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
	bOnline = true;
}

void UObsidianCharacterScreen::InitializeOfflineCharacterScreen()
{
	TabName_TextBlock->SetText(FText::FromString(TEXT("Character Screen")));
	bOnline = false;
}

void UObsidianCharacterScreen::OnPlayClicked()
{
	if (ensure(CachedChosenCharacterEntry))
	{
		if (const UGameInstance* GameInstance = GetGameInstance())
		{
			if (UObsidianSaveGameSubsystem* SaveGameSubsystem = GameInstance->GetSubsystem<UObsidianSaveGameSubsystem>())
			{
				if (OnPlayLoadingFinishedDelegateHandle.IsValid())
				{
					SaveGameSubsystem->OnLoadingFinishedDelegate.Remove(OnPlayLoadingFinishedDelegateHandle);
				}

				OnPlayLoadingFinishedDelegateHandle = SaveGameSubsystem->OnLoadingFinishedDelegate.AddUObject(this,
					&ThisClass::OnPlayHeroLoadFinished);
				SaveGameSubsystem->RequestLoadHeroSaveGameWithID(true, CachedChosenCharacterEntry->GetSaveID(),
					bOnline, GetOwningLocalPlayer<UObsidianLocalPlayer>());
			}
		} 
	}
}

void UObsidianCharacterScreen::OnDeleteClicked()
{
	if(CachedChosenCharacterEntry && FrontEndGameMode && CharacterEntries.Contains(CachedChosenCharacterEntry))
	{
		CharacterEntries.Remove(CachedChosenCharacterEntry);
		CachedChosenCharacterEntry->RemoveFromParent();
		FrontEndGameMode->DeleteHeroClass(CachedChosenCharacterEntry->GetSaveID());

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

void UObsidianCharacterScreen::PopulateCharacterScreen()
{
	if(CharacterEntries.IsEmpty() == false)
	{
		for(UObsidianCharacterEntry* Entry : CharacterEntries)
		{
			Entry->RemoveFromParent();
		}
		CharacterEntries.Reset();
	}
	
	if (const UGameInstance* GameInstance = GetGameInstance())
	{
		if (UObsidianSaveGameSubsystem* SaveGameSubsystem = GameInstance->GetSubsystem<UObsidianSaveGameSubsystem>())
		{
			TArray<FObsidianHeroSaveInfo> SaveInfos;
			const bool bSuccess = SaveGameSubsystem->FillSaveInfosFromMasterSave(bOnline,
				Cast<UObsidianLocalPlayer>(GetOwningLocalPlayer()), /** OUT */ SaveInfos);
			
			if (bSuccess)
			{
				CreateHeroEntries(SaveInfos);
			}
		}	
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

void UObsidianCharacterScreen::CreateHeroEntries(const TArray<FObsidianHeroSaveInfo>& SaveInfos)
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	if(PlayerController == nullptr)
	{
		return;
	}
	
	checkf(CharacterEntryWidgetClass, TEXT("CharacterEntryWidgetClass is invalid in UObsidianCharacterScreen::PopulateCharacterScreen."));
	if (CharacterEntryWidgetClass)
	{
		for (const FObsidianHeroSaveInfo& SaveInfo : SaveInfos)
		{
			UObsidianCharacterEntry* Entry = CreateWidget<UObsidianCharacterEntry>(PlayerController, CharacterEntryWidgetClass);
			Entry->InitializeCharacterEntry(SaveInfo.SaveID, SaveInfo.HeroDescription.HeroName, SaveInfo.HeroDescription.HeroLevel,
				UObsidianGameplayStatics::GetHeroClassText(SaveInfo.HeroDescription.HeroClass),
				false, SaveInfo.HeroDescription.bHardcore);
			
			CharacterList_ScrollBox->AddChild(Entry);
			CharacterEntries.Add(Entry);
		}
	}
		
	InitCharacterScreen();
}

void UObsidianCharacterScreen::OnPlayHeroLoadFinished(UObsidianHeroSaveGame* SaveObject, bool bSuccess)
{
	if (const UGameInstance* GameInstance = GetGameInstance())
	{
		if (UObsidianSaveGameSubsystem* SaveGameSubsystem = GameInstance->GetSubsystem<UObsidianSaveGameSubsystem>())
		{
			SaveGameSubsystem->OnSavingFinishedDelegate.Remove(OnPlayLoadingFinishedDelegateHandle);
		}
	}
	
	if (bSuccess && SaveObject)
	{
		if (bOnline)
		{
			UCommonUIExtensions::PushStreamedContentToLayer_ForPlayer(GetOwningLocalPlayer(),
				ObsidianGameplayTags::UI_Layer_MainMenu, SoftOnlineLobbyWidgetClass);
		}
		else
		{
			UGameplayStatics::OpenLevel(GetWorld(), FName("L_Tutorial_01"));
		}
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

