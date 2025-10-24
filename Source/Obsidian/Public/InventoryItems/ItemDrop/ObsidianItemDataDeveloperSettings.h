// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project
#include "ObsidianTypes/ItemTypes/ObsidianItemTypes.h"

#include "Engine/DeveloperSettings.h"
#include "ObsidianItemDataDeveloperSettings.generated.h"

class UObsidianItemDataConfig;

DECLARE_LOG_CATEGORY_EXTERN(LogItemData, Log, All);

/**
 * 
 */
UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "Item Data Config"))
class OBSIDIAN_API UObsidianItemDataDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UObsidianItemDataDeveloperSettings(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	uint8 GetDefaultDropRollNumberForEntityRarity(const EObsidianEntityRarity EntityRarity) const;
	uint8 GetDefaultAddedTreasureQualityForEntityRarity(const EObsidianEntityRarity EntityRarity) const;
	uint8 GetMaxPrefixCountForRarity(const EObsidianItemRarity ForRarity) const;
	uint8 GetMaxSuffixCountForRarity(const EObsidianItemRarity ForRarity) const;
	uint8 GetMaxAffixCountForRarity(const EObsidianItemRarity ForRarity) const;
	uint8 GetNaturalMinAffixCountForRarity(const EObsidianItemRarity ForRarity) const;
	
public:
	UPROPERTY(Config, EditAnywhere, Category = "Obsidian|Config", meta = (AllowedClasses = "ObsidianItemDataConfig"))
	TSoftObjectPtr<UObsidianItemDataConfig> ItemDataConfig;

	UPROPERTY(Config, EditAnywhere, Category = "Obsidian|Treasure")
	uint8 MaxTreasureQuality = 90;
	
	UPROPERTY(Config, EditAnywhere, Category = "Obsidian|Treasure")
	TMap<EObsidianEntityRarity, uint8> DefaultRarityToNumberOfDropRollsMap;
	
	UPROPERTY(Config, EditAnywhere, Category = "Obsidian|Treasure")
	TMap<EObsidianEntityRarity, uint8> DefaultRarityToAddedTreasureQualityMap;

	UPROPERTY(Config, EditAnywhere, Category = "Obsidian|Treasure")
	TMap<EObsidianItemRarity, uint16> DefaultRarityToWeightMap;

	UPROPERTY(Config, EditAnywhere, Category = "Obsidian|Affixes")
	TSoftObjectPtr<UObsidianAffixAbilitySet> DefaultAffixAbilitySet;
	
	UPROPERTY(Config, EditAnywhere, Category = "Obsidian|Affixes")
	uint8 DefaultMaxImplicitCount = 1;
	
	UPROPERTY(Config, EditAnywhere, Category = "Obsidian|Affixes")
	TMap<EObsidianItemRarity, uint8> DefaultRarityToMaxSuffixCount;

	UPROPERTY(Config, EditAnywhere, Category = "Obsidian|Affixes")
	TMap<EObsidianItemRarity, uint8> DefaultRarityToMaxPrefixCount;

	UPROPERTY(Config, EditAnywhere, Category = "Obsidian|Affixes")
	TMap<EObsidianItemRarity, uint8> DefaultRarityToMaxAffixCount;

	UPROPERTY(Config, EditAnywhere, Category = "Obsidian|Affixes")
	TMap<EObsidianItemRarity, uint8> DefaultRarityToNaturalMinAffixCount;

	
};
