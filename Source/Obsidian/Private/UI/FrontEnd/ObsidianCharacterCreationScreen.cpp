// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/FrontEnd/ObsidianCharacterCreationScreen.h"
#include "Input/CommonUIInputTypes.h"
#include "CommonTextBlock.h"
#include "Components/CheckBox.h"
#include "Components/EditableTextBox.h"
#include "Components/SizeBox.h"
#include "Game/ObsidianFrontEndGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Obsidian/ObsidianGameplayTags.h"
#include "UI/Components/ObsidianButtonBase.h"

void UObsidianCharacterCreationScreen::InitializeCharacterCreationScreen(const bool bIsOnline)
{
	bIsOnlineCharacter = bIsOnline;
	
	const FText TabNameText = bIsOnline ? FText::FromString(TEXT("Online Character Creation")) : FText::FromString(TEXT("Character Creation"));
	TabName_TextBlock->SetText(TabNameText);
}

void UObsidianCharacterCreationScreen::NativeOnActivated()
{
	Super::NativeOnActivated();

	HideHeroDescription();
}

void UObsidianCharacterCreationScreen::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	BackwardsHandle = RegisterUIActionBinding(FBindUIActionArgs(BackwardsInputActionData, false, FSimpleDelegate::CreateUObject(this, &ThisClass::HandleBackwardsAction)));

	FrontEndGameMode = Cast<AObsidianFrontEndGameMode>(UGameplayStatics::GetGameMode(this));
	ensureMsgf(FrontEndGameMode, TEXT("FrontEndGameMode is invalid in UObsidianCharacterCreationScreen::NativeOnActivated()"));
	
	PlayerName_EditableTextBox->OnTextCommitted.AddUniqueDynamic(this, &ThisClass::OnPlayerNameEntered);
	Hardcore_CheckBox->OnCheckStateChanged.AddUniqueDynamic(this, &ThisClass::OnHardcoreCheckboxStatusChanged);

	Create_Button->OnClicked().AddUObject(this, &ThisClass::OnCreateButtonClicked);
	Witch_Button->OnClicked().AddUObject(this, &ThisClass::OnWitchButtonClicked);
	Paladin_Button->OnClicked().AddUObject(this, &ThisClass::OnPaladinButtonClicked);
	Barbarian_Button->OnClicked().AddUObject(this, &ThisClass::OnBarbarianButtonClicked);
	Assassin_Button->OnClicked().AddUObject(this, &ThisClass::OnAssassinButtonClicked);
}

void UObsidianCharacterCreationScreen::HandleBackwardsAction()
{
	if(FrontEndGameMode)
	{
		FrontEndGameMode->ResetCharacterCreation();
	}
	
	DeactivateWidget();
}

void UObsidianCharacterCreationScreen::ShowHeroDescription(const FGameplayTag& HeroTag)
{
	for(const FObsidianHeroInfo& Info : HeroInfos)
	{
		if(Info.HeroTag == HeroTag)
		{
			HeroInfo_HeroName_SizeBox->SetText(Info.HeroName);
			HeroInfo_HeroDescription_SizeBox->SetText(Info.HeroInfoText);
			
			HeroInfo_SizeBox->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

void UObsidianCharacterCreationScreen::HideHeroDescription() const
{
	HeroInfo_SizeBox->SetVisibility(ESlateVisibility::Collapsed);
}

void UObsidianCharacterCreationScreen::OnPlayerNameEntered(const FText& InPlayerName, ETextCommit::Type CommitType)
{
	if(FrontEndGameMode)
	{
		FrontEndGameMode->AssignPlayerName(InPlayerName);
	}
}

void UObsidianCharacterCreationScreen::OnHardcoreCheckboxStatusChanged(bool InBool)
{
	if(FrontEndGameMode)
	{
		FrontEndGameMode->AssignIsHardcore(InBool);
	}
}

void UObsidianCharacterCreationScreen::OnCreateButtonClicked()
{
	if(FrontEndGameMode && ChosenClassTag != FGameplayTag::EmptyTag)
	{
		FrontEndGameMode->CreateHeroClass(ChosenClassTag, bIsOnlineCharacter);
	}
}

void UObsidianCharacterCreationScreen::OnWitchButtonClicked()
{
	ChosenClassTag = ObsidianGameplayTags::Character_Hero_Witch;
}

void UObsidianCharacterCreationScreen::OnPaladinButtonClicked()
{
	ChosenClassTag = ObsidianGameplayTags::Character_Hero_Paladin;
}

void UObsidianCharacterCreationScreen::OnBarbarianButtonClicked()
{
	ChosenClassTag = ObsidianGameplayTags::Character_Hero_Barbarian;
}

void UObsidianCharacterCreationScreen::OnAssassinButtonClicked()
{
	ChosenClassTag = ObsidianGameplayTags::Character_Hero_Assassin;
}

