// Copyright 2024 out of sCope team - Michał Ogiński

#include "UI/FrontEnd/ObsidianCharacterCreationScreen.h"

// ~ Core
#include "CommonTextBlock.h"
#include "CommonUIExtensions.h"
#include "Components/CheckBox.h"
#include "Components/EditableTextBox.h"
#include "Components/SizeBox.h"
#include "Input/CommonUIInputTypes.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"

// ~ Project
#include "Game/ObsidianFrontEndGameMode.h"
#include "Game/Save/ObsidianHeroSaveGame.h"
#include "Game/Save/ObsidianSaveGameSubsystem.h"
#include "UI/Components/ObsidianButtonBase.h"
#include "UI/WidgetControllers/ObCharacterSelectionWidgetController.h"

void UObsidianCharacterCreationScreen::HandleWidgetControllerSet()
{
	CharacterSelectionWidgetController = Cast<UObCharacterSelectionWidgetController>(WidgetController);
	check(CharacterSelectionWidgetController);
}

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
		HandleHoverHeroButton(EObsidianHeroClass::Witch);
	}
	else
	{
		HideHeroDescription();
	}

	HeroDetails_SizeBox->SetVisibility(ESlateVisibility::Collapsed);
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
}

void UObsidianCharacterCreationScreen::HandleBackwardsAction()
{
	ChosenClass = EObsidianHeroClass::None;
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
	FrontEndGameMode = FrontEndGameMode == nullptr ? Cast<AObsidianFrontEndGameMode>(
		UGameplayStatics::GetGameMode(this)) : FrontEndGameMode;

	CharacterSelectionWidgetController->ResetHighlightForCharacterWithTag(ChosenClass);
	HideHeroDescription();

	const ECheckBoxState CheckBoxState = Hardcore_CheckBox->GetCheckedState();
	const bool bIsHardcoreChecked = CheckBoxState == ECheckBoxState::Checked;
	const FObsidianHeroClassParams Params = FrontEndGameMode->CreateHeroClass(ChosenClass,
		PlayerName_EditableTextBox->GetText(), bIsOnlineCharacter, bIsHardcoreChecked);
	
	if(Params.IsValid())
	{
		if (const UGameInstance* GameInstance = GetGameInstance())
		{
			if (UObsidianSaveGameSubsystem* SaveGameSubsystem = GameInstance->GetSubsystem<UObsidianSaveGameSubsystem>())
			{
				if (OnCreateSavingFinishedHandle.IsValid())
				{
					SaveGameSubsystem->OnSavingFinishedDelegate.Remove(OnCreateSavingFinishedHandle);
				}
				
				FObsidianHeroInitializationSaveData InitializationSaveData;
				InitializationSaveData.PlayerHeroName = Params.ObsidianPlayerName;
				InitializationSaveData.HeroObjectClass = Params.HeroObjectClass;
				InitializationSaveData.HeroClass = Params.Class;
				InitializationSaveData.bHardcore = Params.bIsHardcore;

				OnCreateSavingFinishedHandle = SaveGameSubsystem->OnSavingFinishedDelegate.AddUObject(this,
					&ThisClass::OnCreateSavingFinished);
				SaveGameSubsystem->RequestSaveInitialHeroSave(true, bIsOnlineCharacter, InitializationSaveData,
					GetOwningLocalPlayer<UObsidianLocalPlayer>());
			}
		} 
	}
	else
	{
		//TODO(intrxx) Notify user of any errors
	}
}

void UObsidianCharacterCreationScreen::OnWitchButtonClicked()
{
	HandleClickingHeroButton(EObsidianHeroClass::Witch);
	ChosenClass = EObsidianHeroClass::Witch;
}

void UObsidianCharacterCreationScreen::OnWitchButtonHovered()
{
	HandleHoverHeroButton(EObsidianHeroClass::Witch);
}

void UObsidianCharacterCreationScreen::OnWitchButtonUnhovered()
{
	HandleUnhoverHeroButton(EObsidianHeroClass::Witch);
}

void UObsidianCharacterCreationScreen::OnPaladinButtonClicked()
{
	HandleClickingHeroButton(EObsidianHeroClass::Paladin);
	ChosenClass = EObsidianHeroClass::Paladin;
}

void UObsidianCharacterCreationScreen::OnPaladinButtonHovered()
{
	HandleHoverHeroButton(EObsidianHeroClass::Paladin);
}

void UObsidianCharacterCreationScreen::OnPaladinButtonUnhovered()
{
	HandleUnhoverHeroButton(EObsidianHeroClass::Paladin);
}

void UObsidianCharacterCreationScreen::OnBarbarianButtonClicked()
{
	HandleClickingHeroButton(EObsidianHeroClass::Barbarian);
	ChosenClass = EObsidianHeroClass::Barbarian;
}

void UObsidianCharacterCreationScreen::OnBarbarianButtonHovered()
{
	HandleHoverHeroButton(EObsidianHeroClass::Barbarian);
}

void UObsidianCharacterCreationScreen::OnBarbarianButtonUnhovered()
{
	HandleUnhoverHeroButton(EObsidianHeroClass::Barbarian);
}

void UObsidianCharacterCreationScreen::OnAssassinButtonClicked()
{
	HandleClickingHeroButton(EObsidianHeroClass::Assassin);
	ChosenClass = EObsidianHeroClass::Assassin;
}

void UObsidianCharacterCreationScreen::OnAssassinButtonHovered()
{
	HandleHoverHeroButton(EObsidianHeroClass::Assassin);
}

void UObsidianCharacterCreationScreen::OnAssassinButtonUnhovered()
{
	HandleUnhoverHeroButton(EObsidianHeroClass::Assassin);
}

void UObsidianCharacterCreationScreen::HandleClickingHeroButton(const EObsidianHeroClass ForClass)
{
	ResetHeroDetails();
	HeroDetails_SizeBox->SetVisibility(ESlateVisibility::Visible);
	
	if(UCommonUIExtensions::IsOwningPlayerUsingGamepad(this))
	{
		PlayerName_EditableTextBox->SetFocus();
	}
	else
	{
		CharacterSelectionWidgetController->ResetHighlightForCharacterWithTag(ChosenClass);
		CharacterSelectionWidgetController->HighlightCharacterWithTag(ForClass);
		ShowHeroDescription(ForClass);
	}
}

void UObsidianCharacterCreationScreen::HandleHoverHeroButton(const EObsidianHeroClass ForClass)
{
	if(UCommonUIExtensions::IsOwningPlayerUsingGamepad(this))
	{
		if(ChosenClass != EObsidianHeroClass::None)
		{
			ResetHeroDetails();
			HeroDetails_SizeBox->SetVisibility(ESlateVisibility::Collapsed);
			CharacterSelectionWidgetController->ResetHighlightForCharacterWithTag(ChosenClass);
			ChosenClass = EObsidianHeroClass::None;
		}
		CharacterSelectionWidgetController->HighlightCharacterWithTag(ForClass);
		ShowHeroDescription(ForClass);
	}
}

void UObsidianCharacterCreationScreen::HandleUnhoverHeroButton(const EObsidianHeroClass ForClass)
{
	if(UCommonUIExtensions::IsOwningPlayerUsingGamepad(this))
	{
		if(ChosenClass != ForClass)
		{
			CharacterSelectionWidgetController->ResetHighlightForCharacterWithTag(ForClass);
			HideHeroDescription();
		}
	}
}

void UObsidianCharacterCreationScreen::ResetHeroDetails() const
{
	Hardcore_CheckBox->SetCheckedState(ECheckBoxState::Unchecked);
	PlayerName_EditableTextBox->SetText(FText());
}

void UObsidianCharacterCreationScreen::OnCreateSavingFinished(UObsidianHeroSaveGame* SaveGame, bool bSuccess)
{
	if (const UGameInstance* GameInstance = GetGameInstance())
	{
		if (UObsidianSaveGameSubsystem* SaveGameSubsystem = GameInstance->GetSubsystem<UObsidianSaveGameSubsystem>())
		{
			SaveGameSubsystem->OnSavingFinishedDelegate.Remove(OnCreateSavingFinishedHandle);
		}
	}

	if (bSuccess)
	{
		HandleBackwardsAction();
	}
	else
	{
		UE_LOG(LogObsidianSaveSystem, Error, TEXT("Create Save failed in [%hs]"), __FUNCTION__);
	}
}


