// Copyright 2024 out of sCope team - Michał Ogiński

#include "UI/FrontEnd/ObsidianMainMenu.h"

#include <CommonUIExtensions.h>
#include <Kismet/KismetSystemLibrary.h>

#include "Core/FunctionLibraries/ObsidianUIFunctionLibrary.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"

#include "Obsidian/ObsidianGameplayTags.h"
#include "UI/Components/ObsidianButtonBase.h"
#include "UI/FrontEnd/ObsidianCharacterCreationScreen.h"
#include "UI/FrontEnd/ObsidianCharacterScreen.h"
#include "UI/WidgetControllers/ObCharacterSelectionWidgetController.h"

// ~ Core

// ~ Project

UObsidianMainMenu::UObsidianMainMenu(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UObsidianMainMenu::NativeOnActivated()
{
	Super::NativeOnActivated();

	if (UWidget* DesiredFocusTarget = GetDesiredFocusTarget())
	{
		DesiredFocusTarget->SetFocus();
	}
	
	LoadCharacterSelectionClass();
}

void UObsidianMainMenu::NativeConstruct()
{
	Super::NativeConstruct();

	Play_Button->OnClicked().AddUObject(this, &ThisClass::OnPlayClicked);
	Online_Button->OnClicked().AddUObject(this, &ThisClass::OnOnlineClicked);
	Options_Button->OnClicked().AddUObject(this, &ThisClass::OnOptionsClicked);
	Quit_Button->OnClicked().AddUObject(this, &ThisClass::OnQuitClicked);
}

void UObsidianMainMenu::NativeDestruct()
{
	Play_Button->OnClicked().Clear();
	Online_Button->OnClicked().Clear();
	Options_Button->OnClicked().Clear();
	Quit_Button->OnClicked().Clear();
	
	Super::NativeDestruct();
}

UWidget* UObsidianMainMenu::NativeGetDesiredFocusTarget() const
{
	return Play_Button;
}

void UObsidianMainMenu::OnPlayClicked()
{
	const UWorld* World = GetWorld();
	if (World == nullptr)
	{
		return;
	}
	
	if (LoadedCharacterSelectionWidgetClass == nullptr)
	{
		LoadedCharacterSelectionWidgetClass = SoftCharacterSelectionWidgetClass.LoadSynchronous();
		UE_LOG(LogTemp, Warning, TEXT("Needed to load CharacterSelectionWidgetClass Synchronously!"));
	}
	
	UObsidianCharacterScreen* CharacterScreen = Cast<UObsidianCharacterScreen>(
		UCommonUIExtensions::PushContentToLayer_ForPlayer(GetOwningLocalPlayer(),
			ObsidianGameplayTags::UI_Layer_MainMenu,
			LoadedCharacterSelectionWidgetClass));

	if (CharacterScreen)
	{
		CharacterScreen->InitializeOfflineCharacterScreen();
		if (UObCharacterSelectionWidgetController* CharacterSelectionWidgetController =
				UObsidianUIFunctionLibrary::GetCharacterSelectionWidgetController(World))
		{
			CharacterScreen->SetWidgetController(CharacterSelectionWidgetController);
		}
	}
}

void UObsidianMainMenu::OnOnlineClicked()
{
	const UWorld* World = GetWorld();
	if (World == nullptr)
	{
		return;
	}
	
	if (LoadedCharacterSelectionWidgetClass == nullptr)
	{
		LoadedCharacterSelectionWidgetClass = SoftCharacterSelectionWidgetClass.LoadSynchronous();
		UE_LOG(LogTemp, Warning, TEXT("Needed to load CharacterSelectionWidgetClass Synchronously!"));
	}
	
	UObsidianCharacterScreen* CharacterScreen = Cast<UObsidianCharacterScreen>(
		UCommonUIExtensions::PushContentToLayer_ForPlayer(GetOwningLocalPlayer(),
			ObsidianGameplayTags::UI_Layer_MainMenu,
			LoadedCharacterSelectionWidgetClass));

	if (CharacterScreen)
	{
		CharacterScreen->InitializeOnlineCharacterScreen();
		if (UObCharacterSelectionWidgetController* CharacterSelectionWidgetController =
				UObsidianUIFunctionLibrary::GetCharacterSelectionWidgetController(World))
		{
			CharacterScreen->SetWidgetController(CharacterSelectionWidgetController);
		}
	}
}

void UObsidianMainMenu::OnOptionsClicked()
{
	if (SoftSettingsMenuWidgetClass.IsNull() == false)
	{
		UCommonUIExtensions::PushStreamedContentToLayer_ForPlayer(GetOwningLocalPlayer(), ObsidianGameplayTags::UI_Layer_MainMenu,
			SoftSettingsMenuWidgetClass);
	}
}

void UObsidianMainMenu::OnQuitClicked()
{
	if (const UWorld* World = GetWorld())
	{
		UKismetSystemLibrary::QuitGame(World, GetOwningPlayer(), EQuitPreference::Quit, true);
	}
}

void UObsidianMainMenu::LoadCharacterSelectionClass()
{
	if (SoftCharacterSelectionWidgetClass.IsNull() == false)
	{
		const FSoftObjectPath& CharacterSelectionPath = SoftCharacterSelectionWidgetClass.ToSoftObjectPath();
		if (CharacterSelectionPath.IsNull() == false)
		{
			UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(CharacterSelectionPath,
				FStreamableDelegate::CreateLambda([this]()
						{
							LoadedCharacterSelectionWidgetClass = SoftCharacterSelectionWidgetClass.Get();
						})
			);
		}
	}
}
