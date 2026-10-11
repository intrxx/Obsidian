// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CommonButtonBase.h"
#include "CoreMinimal.h"

#include "ObsidianCharacterEntry.generated.h"

class UBorder;
class AObsidianHero;
class UCommonTextBlock;
class UObsidianCharacterEntry;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnEntryClicked, UObsidianCharacterEntry*);

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianCharacterEntry : public UCommonButtonBase
{
	GENERATED_BODY()

public:
	void InitializeCharacterEntry(const uint16 InSaveID, const FString& InPlayerName, const int32 InPlayerLevel, const FText& InPlayerClass,
		const bool InIsOnline, const bool InIsHardcore);

	uint16 GetSaveID() const { return SaveID; }
	bool IsOnline() const { return bOnline; }

	void SetIsChosen();
	void ResetChosenState();

	//~ Begin UUserWidget Interface
	virtual void NativeConstruct() override;
	//~ End UUserWidget Interface

	FOnEntryClicked OnEntryClicked;

protected:
	//~ Begin UCommonButtonBase Interface
	virtual void NativeOnClicked() override;
	//~ End UCommonButtonBase Interface

	uint16 SaveID = INDEX_NONE;
	bool bOnline = false;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UBorder> Chosen_Border;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> PlayerName_TextBlock;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> PlayerLevel_TextBlock;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> PlayerClass_TextBlock;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> PlayType_TextBlock;
};
