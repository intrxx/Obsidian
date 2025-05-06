// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/FrontEnd/ObsidianCharacterCreationScreen.h"
#include "Input/CommonUIInputTypes.h"
#include "CommonTextBlock.h"
#include "Components/CheckBox.h"
#include "Components/EditableTextBox.h"
#include "Components/SizeBox.h"
#include "Game/ObsidianFrontEndGameMode.h"
#include "Kismet/GameplayStatics.h"
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
	ChosenClass = EObsidianHeroClass::OHC_None;
	Hardcore_CheckBox->SetCheckedState(ECheckBoxState::Unchecked);
	PlayerName_EditableTextBox->SetText(FText());
	
	DeactivateWidget();
}

void UObsidianCharacterCreationScreen::ShowHeroDescription(const EObsidianHeroClass& ForClass)
{
	for(const FObsidianHeroInfo& Info : HeroInfos)
	{
		if(Info.Class == ForClass)
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
}

void UObsidianCharacterCreationScreen::OnHardcoreCheckboxStatusChanged(bool InBool)
{
}

void UObsidianCharacterCreationScreen::OnCreateButtonClicked()
{
	if(FrontEndGameMode == nullptr)
	{
		return;
	}

	const ECheckBoxState CheckBoxState = Hardcore_CheckBox->GetCheckedState();
	const bool bIsHardcoreChecked = CheckBoxState == ECheckBoxState::Checked;
	if(FrontEndGameMode->CreateHeroClass(ChosenClass, PlayerName_EditableTextBox->GetText(), bIsOnlineCharacter, bIsHardcoreChecked))
	{
		HandleBackwardsAction();
	}
	else
	{
		// Notify user of any errors
	}
}

void UObsidianCharacterCreationScreen::OnWitchButtonClicked()
{
	ChosenClass = EObsidianHeroClass::OHC_Witch;
}

void UObsidianCharacterCreationScreen::OnPaladinButtonClicked()
{
	ChosenClass = EObsidianHeroClass::OHC_Paladin;
}

void UObsidianCharacterCreationScreen::OnBarbarianButtonClicked()
{
	ChosenClass = EObsidianHeroClass::OHC_Barbarian;
}

void UObsidianCharacterCreationScreen::OnAssassinButtonClicked()
{
	ChosenClass = EObsidianHeroClass::OHC_Assassin;
}

