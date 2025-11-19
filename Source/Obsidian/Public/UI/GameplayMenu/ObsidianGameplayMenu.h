// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include <CoreMinimal.h>

#include "UI/ObsidianActivatableWidget.h"
#include "ObsidianGameplayMenu.generated.h"

class UObsidianButtonBase;
class UCommonActivatableWidget;

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianGameplayMenu : public UObsidianActivatableWidget
{
	GENERATED_BODY()

public:
	virtual UWidget* NativeGetDesiredFocusTarget() const override;
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	void OnContinueClicked();
	void OnOptionsClicked();
	void OnSaveAndExitClicked();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	TSoftClassPtr<UCommonActivatableWidget> SettingsMenuWidgetClass;
	
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UObsidianButtonBase> Continue_Button;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UObsidianButtonBase> Options_Button;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UObsidianButtonBase> SaveAndExit_Button;
};
