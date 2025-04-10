// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "ObsidianMainMenu.generated.h"

class UButton;
class UVerticalBox;
class UCommonTextBlock;

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianMainMenu : public UCommonActivatableWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnPlayButtonClicked();
	UFUNCTION()
	void OnOptionsButtonClicked();
	UFUNCTION()
	void OnExitButtonClicked();
	
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UVerticalBox> MainMenu_VerticalBox;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> Play_TextBlock;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Play_Button;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> Options_TextBlock;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Options_Button;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCommonTextBlock> Exit_TextBlock;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Exit_Button;
};
