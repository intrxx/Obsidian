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
}

void UObsidianMainMenu::NativeConstruct()
{
	Super::NativeConstruct();
	
	Options_Button->OnClicked().AddUObject(this, &ThisClass::OnOptionsClicked);
	Quit_Button->OnClicked().AddUObject(this, &ThisClass::OnQuitClicked);
}

void UObsidianMainMenu::NativeDestruct()
{
	Options_Button->OnClicked().Clear();
	Quit_Button->OnClicked().Clear();
	
	Super::NativeDestruct();
}

UWidget* UObsidianMainMenu::NativeGetDesiredFocusTarget() const
{
	return Play_Button;
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
