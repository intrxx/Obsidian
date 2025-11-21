// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project
#include "ObsidianTypes/ObsidianCoreTypes.h"

#include "UI/ObsidianActivatableWidget.h"
#include "ObsidianCharacterCreationScreen.generated.h"

class UObsidianSaveGame;
class AObsidianFrontEndGameMode;
class UEditableTextBox;
class UCheckBox;
class UObsidianButtonBase;
class USizeBox;
class UCommonTextBlock;

/**
 * 
 */
USTRUCT(BlueprintType)
struct FObsidianHeroInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	EObsidianHeroClass Class = EObsidianHeroClass::None;
	
	UPROPERTY(EditAnywhere)
	FText HeroName = FText();
	
	UPROPERTY(EditAnywhere)
	FText HeroInfoText = FText();

	// Add Image or something
};

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

	virtual UWidget* NativeGetDesiredFocusTarget() const override;

protected:
	virtual void NativeOnActivated() override;
	virtual void NativeOnInitialized() override;

	void HandleBackwardsAction();

	UFUNCTION(BlueprintCallable, Category = "Obsidian")
	void ShowHeroDescription(const EObsidianHeroClass& ForClass);
	UFUNCTION(BlueprintCallable, Category = "Obsidian")
	void HideHeroDescription() const;

	UFUNCTION()
	void OnPlayerNameEntered(const FText& InPlayerName, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnHardcoreCheckboxStatusChanged(bool InBool);
	
	void OnCreateButtonClicked();
	void OnWitchButtonClicked();
	void OnWitchButtonHovered();
	void OnWitchButtonUnhovered();
	void OnPaladinButtonClicked();
	void OnPaladinButtonHovered();
	void OnPaladinButtonUnhovered();
	void OnBarbarianButtonClicked();
	void OnBarbarianButtonHovered();
	void OnBarbarianButtonUnhovered();
	void OnAssassinButtonClicked();
	void OnAssassinButtonHovered();
	void OnAssassinButtonUnhovered();

	void HandleClickingHeroButton(const EObsidianHeroClass ForClass);
	void HandleHoverHeroButton(const EObsidianHeroClass ForClass);
	void HandleUnhoverHeroButton(const EObsidianHeroClass ForClass);

	void ResetHeroDetails() const;

	void OnCreateSavingFinished(UObsidianSaveGame* SaveGame, bool bSuccess);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	TArray<FObsidianHeroInfo> HeroInfos;
	
	UPROPERTY(BlueprintReadOnly, Category = "Obsidian")
	TObjectPtr<AObsidianFrontEndGameMode> FrontEndGameMode;

	/** Class assigned after pressing on the hero button/hero.*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly);
	EObsidianHeroClass ChosenClass = EObsidianHeroClass::None;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Input")
	FDataTableRowHandle BackwardsInputActionData;
	
	FUIActionBindingHandle BackwardsHandle;
	
	bool bIsOnlineCharacter = false;
	bool bInitialGamepadFocusSet = false;

	FDelegateHandle OnCreateSavingFinishedHandle;
	
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> TabName_TextBlock;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USizeBox> HeroDetails_SizeBox;
	
	UPROPERTY(meta=(BindWidget), BlueprintReadOnly)
	TObjectPtr<UEditableTextBox> PlayerName_EditableTextBox;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCheckBox> Hardcore_CheckBox;

	UPROPERTY(meta=(BindWidget), BlueprintReadOnly)
	TObjectPtr<UObsidianButtonBase> Create_Button;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USizeBox> HeroList_SizeBox;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UObsidianButtonBase> Witch_Button;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UObsidianButtonBase> Paladin_Button;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UObsidianButtonBase> Barbarian_Button;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UObsidianButtonBase> Assassin_Button;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USizeBox> HeroInfo_SizeBox;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> HeroInfo_HeroName_SizeBox;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> HeroInfo_HeroDescription_SizeBox;
};

