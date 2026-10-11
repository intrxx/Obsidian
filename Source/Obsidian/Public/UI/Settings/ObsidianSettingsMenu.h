// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"

#include "UI/ObsidianActivatableWidget.h"

#include "ObsidianSettingsMenu.generated.h"

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianSettingsMenu : public UObsidianActivatableWidget
{
	GENERATED_BODY()

protected:
	void HandleBackwardsAction();

	//~ Begin UUserWidget Interface
	virtual void NativeOnInitialized() override;
	//~ End UUserWidget Interface

	UPROPERTY(EditDefaultsOnly)
	FDataTableRowHandle BackwardsInputActionData;

	FUIActionBindingHandle BackwardsHandle;
};
