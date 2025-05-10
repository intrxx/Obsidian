// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/FrontEnd/ObsidianCharacterCreationScreen.h"
#include "Input/CommonUIInputTypes.h"
#include "CommonTextBlock.h"
#include "CommonUIExtensions.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
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

UWidget* UObsidianCharacterCreationScreen::NativeGetDesiredFocusTarget() const
{
	return Witch_Button;
}

void UObsidianCharacterCreationScreen::NativeOnActivated()
{
	Super::NativeOnActivated();
	
	if(UCommonUIExtensions::IsOwningPlayerUsingGamepad(this))
	{
		HandleHoveringHeroButton(EObsidianHeroClass::OHC_Witch);
	}
	else
	{
		HideHeroDescription();
	}
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
	Witch_Button->OnHovered().AddUObject(this, &ThisClass::OnWitchButtonHovered);
	Witch_Button->OnUnhovered().AddUObject(this, &ThisClass::OnWitchButtonUnhovered);
	Paladin_Button->OnClicked().AddUObject(this, &ThisClass::OnPaladinButtonClicked);
	Paladin_Button->OnHovered().AddUObject(this, &ThisClass::OnPaladinButtonHovered);
	Paladin_Button->OnUnhovered().AddUObject(this, &ThisClass::OnPaladinButtonUnhovered);
	Barbarian_Button->OnClicked().AddUObject(this, &ThisClass::OnBarbarianButtonClicked);
	Barbarian_Button->OnHovered().AddUObject(this, &ThisClass::OnBarbarianButtonHovered);
	Barbarian_Button->OnUnhovered().AddUObject(this, &ThisClass::OnBarbarianButtonUnhovered);
	Assassin_Button->OnClicked().AddUObject(this, &ThisClass::OnAssassinButtonClicked);
	Assassin_Button->OnHovered().AddUObject(this, &ThisClass::OnAssassinButtonHovered);
	Assassin_Button->OnUnhovered().AddUObject(this, &ThisClass::OnAssassinButtonUnhovered);

	HeroDetails_SizeBox->SetVisibility(ESlateVisibility::Collapsed);
}

void UObsidianCharacterCreationScreen::HandleBackwardsAction()
{
	ChosenClass = EObsidianHeroClass::OHC_None;
	ResetHeroDetails();
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

	FrontEndGameMode->ResetHighlightForCharacterWithTag(ChosenClass);
	HideHeroDescription();

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
	HandleClickingHeroButton(EObsidianHeroClass::OHC_Witch);
	ChosenClass = EObsidianHeroClass::OHC_Witch;
}

void UObsidianCharacterCreationScreen::OnWitchButtonHovered()
{
	HandleHoveringHeroButton(EObsidianHeroClass::OHC_Witch);
}

void UObsidianCharacterCreationScreen::OnWitchButtonUnhovered()
{
	HandleUnhoverHeroButton(EObsidianHeroClass::OHC_Witch);
}

void UObsidianCharacterCreationScreen::OnPaladinButtonClicked()
{
	HandleClickingHeroButton(EObsidianHeroClass::OHC_Paladin);
	ChosenClass = EObsidianHeroClass::OHC_Paladin;
}

void UObsidianCharacterCreationScreen::OnPaladinButtonHovered()
{
	HandleHoveringHeroButton(EObsidianHeroClass::OHC_Paladin);
}

void UObsidianCharacterCreationScreen::OnPaladinButtonUnhovered()
{
	HandleUnhoverHeroButton(EObsidianHeroClass::OHC_Paladin);
}

void UObsidianCharacterCreationScreen::OnBarbarianButtonClicked()
{
	HandleClickingHeroButton(EObsidianHeroClass::OHC_Barbarian);
	ChosenClass = EObsidianHeroClass::OHC_Barbarian;
}

void UObsidianCharacterCreationScreen::OnBarbarianButtonHovered()
{
	HandleHoveringHeroButton(EObsidianHeroClass::OHC_Barbarian);
}

void UObsidianCharacterCreationScreen::OnBarbarianButtonUnhovered()
{
	HandleUnhoverHeroButton(EObsidianHeroClass::OHC_Barbarian);
}

void UObsidianCharacterCreationScreen::OnAssassinButtonClicked()
{
	HandleClickingHeroButton(EObsidianHeroClass::OHC_Assassin);
	ChosenClass = EObsidianHeroClass::OHC_Assassin;
}

void UObsidianCharacterCreationScreen::OnAssassinButtonHovered()
{
	HandleHoveringHeroButton(EObsidianHeroClass::OHC_Assassin);
}

void UObsidianCharacterCreationScreen::OnAssassinButtonUnhovered()
{
	HandleUnhoverHeroButton(EObsidianHeroClass::OHC_Assassin);
}

void UObsidianCharacterCreationScreen::HandleClickingHeroButton(const EObsidianHeroClass ForClass)
{
	if(FrontEndGameMode == nullptr)
	{
		return;
	}

	ResetHeroDetails();
	HeroDetails_SizeBox->SetVisibility(ESlateVisibility::Visible);
	
	if(UCommonUIExtensions::IsOwningPlayerUsingGamepad(this))
	{
		PlayerName_EditableTextBox->SetFocus();
	}
	else
	{
		FrontEndGameMode->ResetHighlightForCharacterWithTag(ChosenClass);
		FrontEndGameMode->HighlightCharacterWithTag(ForClass);
		ShowHeroDescription(ForClass);
	}
}

void UObsidianCharacterCreationScreen::HandleHoveringHeroButton(const EObsidianHeroClass ForClass)
{
	if(FrontEndGameMode == nullptr)
	{
		return;
	}

	if(UCommonUIExtensions::IsOwningPlayerUsingGamepad(this))
	{
		if(ChosenClass != EObsidianHeroClass::OHC_None)
		{
			ResetHeroDetails();
			HeroDetails_SizeBox->SetVisibility(ESlateVisibility::Collapsed);
			FrontEndGameMode->ResetHighlightForCharacterWithTag(ChosenClass);
			ChosenClass = EObsidianHeroClass::OHC_None;
		}
		FrontEndGameMode->HighlightCharacterWithTag(ForClass);
		ShowHeroDescription(ForClass);
	}
}

void UObsidianCharacterCreationScreen::HandleUnhoverHeroButton(const EObsidianHeroClass ForClass)
{
	if(FrontEndGameMode == nullptr)
	{
		return;
	}

	if(UCommonUIExtensions::IsOwningPlayerUsingGamepad(this))
	{
		if(ChosenClass != ForClass)
		{
			FrontEndGameMode->ResetHighlightForCharacterWithTag(ForClass);
			HideHeroDescription();
		}
	}
}

void UObsidianCharacterCreationScreen::ResetHeroDetails() const
{
	Hardcore_CheckBox->SetCheckedState(ECheckBoxState::Unchecked);
	PlayerName_EditableTextBox->SetText(FText());
}


