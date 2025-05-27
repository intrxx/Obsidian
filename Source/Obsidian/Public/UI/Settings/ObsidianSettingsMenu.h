// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project


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
	virtual void NativeOnInitialized() override;

	void HandleBackwardsAction();

protected:
	UPROPERTY(EditDefaultsOnly)
	FDataTableRowHandle BackwardsInputActionData;
	
	FUIActionBindingHandle BackwardsHandle;
	
};
