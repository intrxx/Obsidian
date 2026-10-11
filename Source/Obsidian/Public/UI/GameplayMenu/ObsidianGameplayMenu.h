// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"

#include "UI/ObsidianActivatableWidget.h"

#include "ObsidianGameplayMenu.generated.h"

class UObsidianHeroSaveGame;
class UObsidianButtonBase;
class UCommonActivatableWidget;

/**
 * 
 */
UCLASS(meta = (PrioritizeCategories = "Obsidian"))
class OBSIDIAN_API UObsidianGameplayMenu : public UObsidianActivatableWidget
{
	GENERATED_BODY()

public:

protected:
	void OnContinueClicked();
	void OnOptionsClicked();
	void OnSaveAndExitClicked();

	void OnSaveFinished(UObsidianHeroSaveGame* InSaveGame, bool bInSuccess);

	//~ Begin UCommonActivatableWidget Interface
	virtual UWidget* NativeGetDesiredFocusTarget() const override;
	//~ End UCommonActivatableWidget Interface

	//~ Begin UUserWidget Interface
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	//~ End UUserWidget Interface

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	TSoftClassPtr<UCommonActivatableWidget> SettingsMenuWidgetClass;

	FDelegateHandle OnSaveFinishedDelegateHandle;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UObsidianButtonBase> Continue_Button;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UObsidianButtonBase> Options_Button;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UObsidianButtonBase> SaveAndExit_Button;
};
