// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/FrontEnd/ObsidianCharacterEntry.h"

#include "CommonTextBlock.h"

void UObsidianCharacterEntry::InitializeCharacterEntry(const FText& InPlayerName, const int32 InPlayerLevel, const FText& InPlayerClass,
                                                       const bool InIsOnline, const bool InIsHardcore)
{
	PlayerName_TextBlock->SetText(InPlayerName);
	PlayerLevel_TextBlock->SetText(FText::FromString(FString::Printf(TEXT("%d"), InPlayerLevel)));
	PlayerClass_TextBlock->SetText(InPlayerClass);

	const FString IsOnlineString = InIsOnline ? TEXT("Online") : TEXT("Offline");
	const FString IsHardcoreString = InIsHardcore ? TEXT("Hardcore") : TEXT("Softcore");
	const FText PlayTypeText = FText::FromString(FString::Printf(TEXT("%s %s"), *IsOnlineString, *IsHardcoreString));
	PlayType_TextBlock->SetText(PlayTypeText);
}

void UObsidianCharacterEntry::NativeOnClicked()
{
	if(!GetLocked())
	{
		OnEntryClicked.Broadcast(this);
	}
	
	Super::NativeOnClicked();
}
