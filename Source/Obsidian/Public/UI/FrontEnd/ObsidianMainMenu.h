// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "UI/ObsidianActivatableWidget.h"
#include "ObsidianMainMenu.generated.h"

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianMainMenu : public UObsidianActivatableWidget
{
	GENERATED_BODY()

public:
	UObsidianMainMenu(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	TSoftClassPtr<UCommonActivatableWidget> SettingsMenuWidgetClass;
};
