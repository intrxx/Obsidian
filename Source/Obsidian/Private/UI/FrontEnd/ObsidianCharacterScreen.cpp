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

	UE_LOG(LogTemp, Warning, TEXT("NativeConstruct"));

	if(Play_Button)
	{
		Play_Button->OnClicked().AddUObject(this, &ThisClass::OnPlayClicked);
	}
	
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
			
			UGameplayStatics::OpenLevel(World, FName("L_TestMap"));
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
}

void UObsidianCharacterScreen::HandleClickingOnCharacterEntry(UObsidianCharacterEntry* EntryClicked)
{
	if(FrontEndGameMode)
	{
		FrontEndGameMode->ChosenHeroClass = EntryClicked->TempObsidianHeroClass;
	}
}
