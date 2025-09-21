// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project

#include "Engine/DeveloperSettings.h"
#include "ObsidianItemDataDeveloperSettings.generated.h"

class UObsidianTreasureConfig;

/**
 * 
 */
UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "Item Data Config"))
class OBSIDIAN_API UObsidianItemDataDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, Category = "Config", meta = (AllowedClasses = "ObsidianTreasureConfig"))
	TSoftObjectPtr<UObsidianTreasureConfig> TreasureConfig;
};
