// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project

#include "Engine/DataAsset.h"
#include "ObsidianItemDataConfig.generated.h"

class UObsidianAffixList;
class UObsidianTreasureList;

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianItemDataConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Treasure")
	TArray<TObjectPtr<UObsidianTreasureList>> CommonTreasureLists;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Treasure")
	TArray<TObjectPtr<UObsidianTreasureList>> UniqueTreasureLists;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Treasure")
	TArray<TObjectPtr<UObsidianTreasureList>> SetTreasureLists;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Affixes")
	TArray<TObjectPtr<UObsidianAffixList>> CommonAffixLists;
};
