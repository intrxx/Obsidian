// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/FrontEnd/ObsidianCharacterCreationScreen.h"

#include "CommonTextBlock.h"

void UObsidianCharacterCreationScreen::InitializeCharacterCreationScreen(const bool bIsOnline)
{
	bIsOnlineCharacter = bIsOnline;
	
	const FText TabNameText = bIsOnline ? FText::FromString(TEXT("Online Character Creation")) : FText::FromString(TEXT("Character Creation"));
	TabName_TextBlock->SetText(TabNameText);
}
