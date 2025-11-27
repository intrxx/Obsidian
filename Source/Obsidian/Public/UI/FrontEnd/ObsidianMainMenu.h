// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include <CoreMinimal.h>


#include "UI/ObsidianActivatableWidget.h"
#include "ObsidianMainMenu.generated.h"

class UObsidianButtonBase;
/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianMainMenu : public UObsidianActivatableWidget
{
	GENERATED_BODY()

public:
	UObsidianMainMenu(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual UWidget* NativeGetDesiredFocusTarget() const override;

protected:
	virtual void NativeOnActivated() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
	void OnOptionsClicked();
	void OnQuitClicked();
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	TSoftClassPtr<UCommonActivatableWidget> SoftSettingsMenuWidgetClass;

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UObsidianButtonBase> Play_Button;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UObsidianButtonBase> Online_Button;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UObsidianButtonBase> Options_Button;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UObsidianButtonBase> Quit_Button;

private:
	void LoadCharacterSelectionClass();
};
