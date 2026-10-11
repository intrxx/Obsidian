// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"

#include "UI/ObsidianActivatableWidget.h"

#include "ObsidianMainMenu.generated.h"

class UObsidianButtonBase;
/**
 * 
 */
UCLASS(meta = (PrioritizeCategories = "Obsidian"))
class OBSIDIAN_API UObsidianMainMenu : public UObsidianActivatableWidget
{
	GENERATED_BODY()

public:
	UObsidianMainMenu(const FObjectInitializer& InObjectInitializer = FObjectInitializer::Get());

protected:
	void OnOptionsClicked();
	void OnQuitClicked();

	//~ Begin UCommonActivatableWidget Interface
	virtual UWidget* NativeGetDesiredFocusTarget() const override;

	virtual void NativeOnActivated() override;
	//~ End UCommonActivatableWidget Interface

	//~ Begin UUserWidget Interface
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	//~ End UUserWidget Interface

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	TSoftClassPtr<UCommonActivatableWidget> SoftSettingsMenuWidgetClass;
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
