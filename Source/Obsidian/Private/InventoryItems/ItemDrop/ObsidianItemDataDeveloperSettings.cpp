// Copyright 2026 out of sCope team - intrxx

#include "InventoryItems/ItemDrop/ObsidianItemDataDeveloperSettings.h"

// ~ Core

// ~ Project

DEFINE_LOG_CATEGORY(LogItemData);

UObsidianItemDataDeveloperSettings::UObsidianItemDataDeveloperSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

uint8 UObsidianItemDataDeveloperSettings::GetDefaultDropRollNumberForEntityRarity(const EObsidianEntityRarity EntityRarity) const
{
	if (const uint8* CountPtr = DefaultRarityToNumberOfDropRollsMap.Find(EntityRarity))
	{
		return *CountPtr;
	}
	UE_LOG(LogItemData, Error, TEXT("Provided EntityRarity [%d] is not included in the DefaultRarityToNumberOfDropRollsMap!"), EntityRarity)
	return 0;
}

uint8 UObsidianItemDataDeveloperSettings::GetDefaultAddedTreasureQualityForEntityRarity(const EObsidianEntityRarity EntityRarity) const
{
	if (const uint8* CountPtr = DefaultRarityToAddedTreasureQualityMap.Find(EntityRarity))
	{
		return *CountPtr;
	}
	UE_LOG(LogItemData, Error, TEXT("Provided EntityRarity [%d] is not included in the DefaultRarityToAddedTreasureQualityMap!"), EntityRarity)
	return 0;
}

uint8 UObsidianItemDataDeveloperSettings::GetMaxPrefixCountForRarity(const EObsidianItemRarity ForRarity) const
{
	if (const uint8* CountPtr = DefaultRarityToMaxPrefixCount.Find(ForRarity))
	{
		return *CountPtr;
	}
	UE_LOG(LogItemData, Error, TEXT("Provided Rarity is not included in the DefaultRarityToMaxPrefixCount!"));
	return 0;
}

uint8 UObsidianItemDataDeveloperSettings::GetMaxSuffixCountForRarity(const EObsidianItemRarity ForRarity) const
{
	if (const uint8* CountPtr = DefaultRarityToMaxSuffixCount.Find(ForRarity))
	{
		return *CountPtr;
	}
	UE_LOG(LogItemData, Error, TEXT("Provided Rarity is not included in the DefaultRarityToMaxSuffixCount!"));
	return 0;
}

uint8 UObsidianItemDataDeveloperSettings::GetMaxAffixCountForRarity(const EObsidianItemRarity ForRarity) const
{
	if (const uint8* CountPtr = DefaultRarityToMaxAffixCount.Find(ForRarity))
	{
		return *CountPtr;
	}
	UE_LOG(LogItemData, Error, TEXT("Provided RarityTag is not included in the DefaultRarityToMaxAffixCount!"));
	return 0;
}

uint8 UObsidianItemDataDeveloperSettings::GetNaturalMinAffixCountForRarity(const EObsidianItemRarity ForRarity) const
{
	if (const uint8* CountPtr = DefaultRarityToNaturalMinAffixCount.Find(ForRarity))
	{
		return *CountPtr;
	}
	UE_LOG(LogItemData, Error, TEXT("Provided RarityTag is not included in the DefaultRarityToNaturalMinAffixCount!"));
	return 0;
}
