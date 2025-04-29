// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/FrontEnd/ObsidianCharacterScreen.h"
#include "CommonTextBlock.h"
#include "CommonUIExtensions.h"
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

	if(Play_Button)
	{
		Play_Button->OnClicked().AddUObject(this, &ThisClass::OnPlayClicked);
	}
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
			UGameplayStatics::OpenLevel(World, FName("L_TestMap"));
		}
	}
}
