// Copyright 2024 out of sCope team - Michał Ogiński

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

uint8 UObsidianItemDataDeveloperSettings::GetMaxPrefixCountForRarity(const FGameplayTag& RarityTag) const
{
	if (const uint8* CountPtr = DefaultRarityToMaxPrefixCount.Find(RarityTag))
	{
		return *CountPtr;
	}
	UE_LOG(LogItemData, Error, TEXT("Provided RarityTag [%s] is not included in the DefaultRarityToMaxPrefixCount!"),
		*RarityTag.GetTagName().ToString())
	return 0;
}

uint8 UObsidianItemDataDeveloperSettings::GetMaxSuffixCountForRarity(const FGameplayTag& RarityTag) const
{
	if (const uint8* CountPtr = DefaultRarityToMaxSuffixCount.Find(RarityTag))
	{
		return *CountPtr;
	}
	UE_LOG(LogItemData, Error, TEXT("Provided RarityTag [%s] is not included in the DefaultRarityToMaxSuffixCount!"),
		*RarityTag.GetTagName().ToString())
	return 0;
}

uint8 UObsidianItemDataDeveloperSettings::GetMaxAffixCountForRarity(const FGameplayTag& RarityTag) const
{
	if (const uint8* CountPtr = DefaultRarityToMaxAffixCount.Find(RarityTag))
	{
		return *CountPtr;
	}
	UE_LOG(LogItemData, Error, TEXT("Provided RarityTag [%s] is not included in the DefaultRarityToMaxAffixCount!"),
		*RarityTag.GetTagName().ToString())
	return 0;
}

uint8 UObsidianItemDataDeveloperSettings::GetNaturalMinAffixCountForRarity(const FGameplayTag& RarityTag) const
{
	if (const uint8* CountPtr = DefaultRarityToNaturalMinAffixCount.Find(RarityTag))
	{
		return *CountPtr;
	}
	UE_LOG(LogItemData, Error, TEXT("Provided RarityTag [%s] is not included in the DefaultRarityToNaturalMinAffixCount!"),
		*RarityTag.GetTagName().ToString())
	return 0;
}
