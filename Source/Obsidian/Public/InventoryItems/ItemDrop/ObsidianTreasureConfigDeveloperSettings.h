// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project

#include "Engine/DeveloperSettings.h"
#include "ObsidianTreasureConfigDeveloperSettings.generated.h"

class UObsidianTreasureConfig;

/**
 * 
 */
UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "Treasure Config"))
class OBSIDIAN_API UObsidianTreasureConfigDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, Category = "Config", meta = (AllowedClasses = "ObsidianTreasureConfig"))
	TSoftObjectPtr<UObsidianTreasureConfig> TreasureConfig;
};
