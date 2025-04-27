// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "ObsidianMainMenu.generated.h"

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianMainMenu : public UCommonActivatableWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	TSoftClassPtr<UCommonActivatableWidget> SettingsMenuWidgetClass;
	
};
