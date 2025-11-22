// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include <CoreMinimal.h>

#include "UI/ObsidianWidgetControllerBase.h"
#include "ObCharacterSelectionWidgetController.generated.h"

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObCharacterSelectionWidgetController : public UObsidianHeroWidgetControllerBase
{
	GENERATED_BODY()

public:
	// ~ Start of WidgetControllerBase
	void OnWidgetControllerSetupCompleted() override;
	// ~ End of WidgetControllerBase
};
