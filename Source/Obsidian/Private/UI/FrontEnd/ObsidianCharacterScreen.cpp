// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/FrontEnd/ObsidianCharacterScreen.h"
#include "UI/FrontEnd/ObsidianCharacterEntry.h"
#include "CommonHierarchicalScrollBox.h"
#include "CommonTextBlock.h"
#include "CommonUIExtensions.h"
#include "Core/ObsidianGameplayStatics.h"
#include "Game/ObsidianFrontEndGameMode.h"
#include "Game/ObsidianGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Obsidian/ObsidianGameplayTags.h"
#include "UI/Components/ObsidianButtonBase.h"

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
	if(Play_Button)
	{
		Play_Button->OnClicked().Clear();
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
		FrontEndGameMode->ChosenHeroClass = nullptr;
	}
	
	Super::NativeOnDeactivated();
}

void UObsidianCharacterScreen::OnPlayClicked()
{
	if(bIsOnline)
	{
		UCommonUIExtensions::PushContentToLayer_ForPlayer(GetOwningLocalPlayer(), ObsidianGameplayTags::UI_Layer_MainMenu, OnlineLobbyWidgetClass);
	}
	else
	{
		if(UWorld* World = GetWorld())
		{
			if(UObsidianGameInstance* ObsidianGameInstance = Cast<UObsidianGameInstance>(UGameplayStatics::GetGameInstance(World)))
			{
				ObsidianGameInstance->ChosenHero = FrontEndGameMode->ChosenHeroClass;
			}
			
			UGameplayStatics::OpenLevel(World, FName("L_Tutorial_01"));
		}
	}
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
				FrontEndGameMode->ChosenHeroClass = NewEntry->TempObsidianHeroClass;
				CachedChosenCharacterEntry = NewEntry;
			}
		}
		else
		{
			FrontEndGameMode->ChosenHeroClass = nullptr;
			Play_Button->SetIsEnabled(false);
		}
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
	
	if(bIsOnline)
	{
		
	}
	else
	{
		// Get Saved Heroes

		// Get Created Heroes, should automatically be saved later so it won't matter
		for(const FObsidianHeroClassParams Params : FrontEndGameMode->GetCreatedHeroes())
		{
			checkf(CharacterEntryWidgetClass, TEXT("CharacterEntryWidgetClass is invalid in UObsidianCharacterScreen::PopulateCharacterScreen."));
			UObsidianCharacterEntry* Entry = CreateWidget<UObsidianCharacterEntry>(PlayerController, CharacterEntryWidgetClass);
			Entry->InitializeCharacterEntry(Params.ObsidianPlayerName, 1, UObsidianGameplayStatics::GetHeroClassText(Params.Class), false, Params.bIsHardcore);
			Entry->TempObsidianHeroClass = Params.SoftHeroClass;
			Entry->TempSaveID = Params.TempID;
			
			CharacterList_ScrollBox->AddChild(Entry);
			CharacterEntries.Add(Entry);
		}
	}

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
		FrontEndGameMode->ChosenHeroClass = CachedChosenCharacterEntry->TempObsidianHeroClass;

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
	
	if(FrontEndGameMode)
	{
		FrontEndGameMode->ChosenHeroClass = EntryClicked->TempObsidianHeroClass;
	}
}
