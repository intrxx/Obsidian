// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project

#include "Engine/DataAsset.h"
#include "ObsidianTypes/ItemTypes/ObsidianItemAffixTypes.h"
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
	UObsidianItemDataConfig(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	FString GetRandomItemNameAddition(const int32 UpToTreasureQuality, const FGameplayTag& ForItemCategoryTag);
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Items")
	FObsidianRareItemNameGenerationData RareItemNameGenerationData;

	/** Unique addition to Magic Item that was rolled with increased Affix multiplier. */
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Items")
	FString MultiplierItemNameAddition = FString(TEXT("High"));
	
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Treasure")
	TArray<TObjectPtr<UObsidianTreasureList>> CommonTreasureLists;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Treasure")
	TArray<TObjectPtr<UObsidianTreasureList>> UniqueTreasureLists;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Treasure")
	TArray<TObjectPtr<UObsidianTreasureList>> SetTreasureLists;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Affixes")
	TArray<TObjectPtr<UObsidianAffixList>> CommonAffixLists;
};
