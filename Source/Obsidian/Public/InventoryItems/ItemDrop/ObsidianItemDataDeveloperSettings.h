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

	FORCEINLINE uint8 GetDefaultDropRollNumberForEntityRarity(const EObsidianEntityRarity EntityRarity) const;
	FORCEINLINE uint8 GetDefaultAddedTreasureQualityForEntityRarity(const EObsidianEntityRarity EntityRarity) const;
	FORCEINLINE uint8 GetMaxPrefixCountForRarity(const FGameplayTag& RarityTag) const;
	FORCEINLINE uint8 GetMaxSuffixCountForRarity(const FGameplayTag& RarityTag) const;
	FORCEINLINE uint8 GetMaxAffixCountForRarity(const FGameplayTag& RarityTag) const;
	FORCEINLINE uint8 GetNaturalMinAffixCountForRarity(const FGameplayTag& RarityTag) const;
	
public:
	UPROPERTY(Config, EditAnywhere, Category = "Obsidian|Config", meta = (AllowedClasses = "ObsidianItemDataConfig"))
	TSoftObjectPtr<UObsidianItemDataConfig> ItemDataConfig;

	UPROPERTY(Config, EditAnywhere, Category = "Obsidian|Treasure")
	uint8 MaxTreasureQuality = 90;

	UPROPERTY(Config, EditAnywhere, Category = "Obsidian|Affixes")
	uint8 DefaultMaxImplicitCount = 1;
	
	UPROPERTY(Config, EditAnywhere, Category = "Obsidian|Treasure")
	TMap<EObsidianEntityRarity, uint8> DefaultRarityToNumberOfDropRollsMap;
	
	UPROPERTY(Config, EditAnywhere, Category = "Obsidian|Treasure")
	TMap<EObsidianEntityRarity, uint8> DefaultRarityToAddedTreasureQualityMap;

	UPROPERTY(Config, EditAnywhere, Category = "Obsidian|Treasure")
	TMap<FGameplayTag, uint16> DefaultRarityToWeightMap;

	UPROPERTY(Config, EditAnywhere, Category = "Obsidian|Affixes")
	TMap<FGameplayTag, uint8> DefaultRarityToMaxSuffixCount;

	UPROPERTY(Config, EditAnywhere, Category = "Obsidian|Affixes")
	TMap<FGameplayTag, uint8> DefaultRarityToMaxPrefixCount;

	UPROPERTY(Config, EditAnywhere, Category = "Obsidian|Affixes")
	TMap<FGameplayTag, uint8> DefaultRarityToMaxAffixCount;

	UPROPERTY(Config, EditAnywhere, Category = "Obsidian|Affixes")
	TMap<FGameplayTag, uint8> DefaultRarityToNaturalMinAffixCount;

	
};
