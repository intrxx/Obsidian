// Copyright 2026 out of sCope team - intrxx

#include "InventoryItems/ItemDrop/ObsidianItemDataDeveloperSettings.h"

#include "Obsidian/ObsidianLogCategories.h"


UObsidianItemDataDeveloperSettings::UObsidianItemDataDeveloperSettings(const FObjectInitializer& InObjectInitializer)
	: Super(InObjectInitializer)
{
}

uint8 UObsidianItemDataDeveloperSettings::GetDefaultDropRollNumberForEntityRarity(const EObsidianEntityRarity InEntityRarity) const
{
	if (const uint8* CountPtr = DefaultRarityToNumberOfDropRollsMap.Find(InEntityRarity))
	{
		return *CountPtr;
	}
	UE_LOG(ObLogItemData, Error, TEXT("Provided EntityRarity [%d] is not included in the DefaultRarityToNumberOfDropRollsMap!"), InEntityRarity)
	return 0;
}

uint8 UObsidianItemDataDeveloperSettings::GetDefaultAddedTreasureQualityForEntityRarity(const EObsidianEntityRarity InEntityRarity) const
{
	if (const uint8* CountPtr = DefaultRarityToAddedTreasureQualityMap.Find(InEntityRarity))
	{
		return *CountPtr;
	}
	UE_LOG(ObLogItemData, Error, TEXT("Provided EntityRarity [%d] is not included in the DefaultRarityToAddedTreasureQualityMap!"), InEntityRarity)
	return 0;
}

uint8 UObsidianItemDataDeveloperSettings::GetMaxPrefixCountForRarity(const EObsidianItemRarity InForRarity) const
{
	if (const uint8* CountPtr = DefaultRarityToMaxPrefixCount.Find(InForRarity))
	{
		return *CountPtr;
	}
	UE_LOG(ObLogItemData, Error, TEXT("Provided Rarity is not included in the DefaultRarityToMaxPrefixCount!"));
	return 0;
}

uint8 UObsidianItemDataDeveloperSettings::GetMaxSuffixCountForRarity(const EObsidianItemRarity InForRarity) const
{
	if (const uint8* CountPtr = DefaultRarityToMaxSuffixCount.Find(InForRarity))
	{
		return *CountPtr;
	}
	UE_LOG(ObLogItemData, Error, TEXT("Provided Rarity is not included in the DefaultRarityToMaxSuffixCount!"));
	return 0;
}

uint8 UObsidianItemDataDeveloperSettings::GetMaxAffixCountForRarity(const EObsidianItemRarity InForRarity) const
{
	if (const uint8* CountPtr = DefaultRarityToMaxAffixCount.Find(InForRarity))
	{
		return *CountPtr;
	}
	UE_LOG(ObLogItemData, Error, TEXT("Provided RarityTag is not included in the DefaultRarityToMaxAffixCount!"));
	return 0;
}

uint8 UObsidianItemDataDeveloperSettings::GetNaturalMinAffixCountForRarity(const EObsidianItemRarity InForRarity) const
{
	if (const uint8* CountPtr = DefaultRarityToNaturalMinAffixCount.Find(InForRarity))
	{
		return *CountPtr;
	}
	UE_LOG(ObLogItemData, Error, TEXT("Provided RarityTag is not included in the DefaultRarityToNaturalMinAffixCount!"));
	return 0;
}

TArray<uint8> UObsidianItemDataDeveloperSettings::GetAffixNumberWeightsForRarity(
	const EObsidianItemRarity InForRarity) const
{
	if (const FObsidianWeightsWrapper* WeightsWrapper = DefaultRarityToNumberOfAffixesWeights.Find(InForRarity))
	{
		return WeightsWrapper->Weights;
	}
	UE_LOG(ObLogItemData, Error, TEXT("Provided RarityTag is not included in the DefaultRarityToNumberOfAffixesWeights!"));
	return {};
}
