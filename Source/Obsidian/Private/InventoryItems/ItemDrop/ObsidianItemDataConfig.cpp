// Copyright 2026 out of sCope team - intrxx

#include "InventoryItems/ItemDrop/ObsidianItemDataConfig.h"


UObsidianItemDataConfig::UObsidianItemDataConfig(const FObjectInitializer& InObjectInitializer)
	: Super(InObjectInitializer)
{
}

FString UObsidianItemDataConfig::GetRandomItemNameAddition(const int32 InUpToTreasureQuality, const FGameplayTag& InForItemCategoryTag)
{
	if (InForItemCategoryTag.IsValid() == false || InUpToTreasureQuality < 0)
	{
		return FString();
	}
	
	return RareItemNameGenerationData.GetRandomPrefixNameAddition(InUpToTreasureQuality).ToString() +
		FString::Printf(TEXT(" %s"), *RareItemNameGenerationData.GetRandomSuffixNameAddition(InUpToTreasureQuality, InForItemCategoryTag).ToString());
}

