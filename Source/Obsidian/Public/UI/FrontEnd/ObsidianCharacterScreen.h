// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "UI/ObsidianActivatableWidget.h"
#include "ObsidianCharacterScreen.generated.h"

class UObsidianButtonBase;
/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianCharacterScreen : public UObsidianActivatableWidget
{
	GENERATED_BODY()

public:
	virtual UWidget* NativeGetDesiredFocusTarget() const override;

protected:
	virtual void NativeConstruct() override;

	void OnPlayClicked();
	
protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UObsidianButtonBase> Play_Button;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UObsidianButtonBase> Create_Button;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UObsidianButtonBase> Delete_Button;
};
