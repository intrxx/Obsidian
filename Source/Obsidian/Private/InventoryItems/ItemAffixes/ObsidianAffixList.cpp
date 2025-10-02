// Copyright 2024 out of sCope team - Michał Ogiński

#include "InventoryItems/ItemAffixes/ObsidianAffixList.h"

// ~ Core

// ~ Project

// ~ FObsidianAffixClass

TArray<FObsidianDynamicItemAffix> FObsidianAffixClass::GetAllAffixesUpToQuality(const int32 UpToTreasureQuality) const
{
	TArray<FObsidianDynamicItemAffix> MatchingTreasureClasses;
	
	for (const FObsidianDynamicItemAffix& Class : ItemAffixList)
	{
		if (Class.MinItemLevelRequirement <= UpToTreasureQuality)
		{
			MatchingTreasureClasses.Add(Class);
		}
	}

	return MatchingTreasureClasses;
}

TArray<FObsidianDynamicItemAffix> FObsidianAffixClass::GetAllAffixesUpToQualityForCategory(const int32 UpToTreasureQuality, const FGameplayTag& ForCategory) const
{
	TArray<FObsidianDynamicItemAffix> MatchingTreasureClasses;
	
	for (const FObsidianDynamicItemAffix& Class : ItemAffixList)
	{
		if (Class.MinItemLevelRequirement <= UpToTreasureQuality && Class.AcceptedItemCategories.HasTagExact(ForCategory))
		{
			MatchingTreasureClasses.Add(Class);
		}
	}

	return MatchingTreasureClasses;
}

// ~ FObsidianAffixClass

UObsidianAffixList::UObsidianAffixList(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UObsidianAffixList::PostInitProperties()
{
	Super::PostInitProperties();

	for (FObsidianAffixClass& AffixClass : AffixClasses)
	{
		AffixClass.ItemAffixList.Sort([](const FObsidianDynamicItemAffix& A, const FObsidianDynamicItemAffix& B)
				{
					return A.MinItemLevelRequirement > B.MinItemLevelRequirement;
				});
	}
}

void UObsidianAffixList::PostLoad()
{
	Super::PostLoad();
}

TArray<FObsidianAffixClass> UObsidianAffixList::GetAllAffixClasses() const
{
	return AffixClasses;
}

