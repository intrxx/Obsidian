// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "UI/ObsidianActivatableWidget.h"
#include "ObsidianCharacterCreationScreen.generated.h"

class UEditableTextBox;
class UCheckBox;
class UObsidianButtonBase;
class USizeBox;
class UCommonTextBlock;
/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianCharacterCreationScreen : public UObsidianActivatableWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Obsidian|CharacterCreation")
	void InitializeCharacterCreationScreen(const bool bIsOnline);

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> TabName_TextBlock;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USizeBox> CharacterDetails_SizeBox;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UEditableTextBox> CharacterName_EditableTextBox;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCheckBox> Hardcore_CheckBox;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USizeBox> CharacterList_SizeBox;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UObsidianButtonBase> Witch_Button;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UObsidianButtonBase> Paladin_Button;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UObsidianButtonBase> Barbarian_Button;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UObsidianButtonBase> Assassin_Button;
	
protected:
	bool bIsOnlineCharacter = false;
};
