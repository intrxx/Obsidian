// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "CommonButtonBase.h"
#include "ObsidianCharacterEntry.generated.h"

class UCommonTextBlock;
/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianCharacterEntry : public UCommonButtonBase
{
	GENERATED_BODY()

public:
	void InitializeCharacterEntry(const FText& InPlayerName, const int32 InPlayerLevel, const FText& InPlayerClass, const bool InIsOnline, const bool InIsHardcore);

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> PlayerName_TextBlock;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> PlayerLevel_TextBlock;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> PlayerClass_TextBlock;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> PlayType_TextBlock;
};
