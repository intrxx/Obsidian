// Copyright 2026 out of sCope team - intrxx

#include "InventoryItems/ItemDrop/ObsidianItemDataConfig.h"


UObsidianItemDataConfig::UObsidianItemDataConfig(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

FString UObsidianItemDataConfig::GetRandomItemNameAddition(const int32 UpToTreasureQuality, const FGameplayTag& ForItemCategoryTag)
{
	if (ForItemCategoryTag.IsValid() == false || UpToTreasureQuality < 0)
	{
		return FString();
	}
	
	return RareItemNameGenerationData.GetRandomPrefixNameAddition(UpToTreasureQuality).ToString() +
		FString::Printf(TEXT(" %s"), *RareItemNameGenerationData.GetRandomSuffixNameAddition(UpToTreasureQuality, ForItemCategoryTag).ToString());
}

