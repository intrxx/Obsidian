// Copyright 2024 out of sCope team - Michał Ogiński

#include "ObsidianTypes/ItemTypes/ObsidianItemTypes.h"

// ~ Core

// ~ Project
#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "Obsidian/ObsidianGameModule.h"

// ~ ObsidianTreasureStatics

namespace ObsidianTreasureStatics
{
	const uint8 MaxTreasureQuality = 90;
	const TMap<EObsidianEntityRarity, uint8> DefaultRarityToNumberOfDropRollsMap =
		{
			{ EObsidianEntityRarity::Neutral, 1 },
			{ EObsidianEntityRarity::Normal, 1 },
			{ EObsidianEntityRarity::Magic, 2 },
			{ EObsidianEntityRarity::Rare, 3 },
			{ EObsidianEntityRarity::MiniBoss, 4 },
			{ EObsidianEntityRarity::MiniBossCompanion, 3 },
			{ EObsidianEntityRarity::SpecialBoss, 5 }
		};

	const TMap<EObsidianEntityRarity, uint8> DefaultRarityToAddedTreasureQualityMap =
		{
			{ EObsidianEntityRarity::Neutral, 0 },
			{ EObsidianEntityRarity::Normal, 0 },
			{ EObsidianEntityRarity::Magic, 1 },
			{ EObsidianEntityRarity::Rare, 3 },
			{ EObsidianEntityRarity::MiniBoss, 4 },
			{ EObsidianEntityRarity::MiniBossCompanion, 4 },
			{ EObsidianEntityRarity::SpecialBoss, 5 }
		};

	const TMap<FGameplayTag, uint16> DefaultRarityToWeightMap =
		{
			{ ObsidianGameplayTags::Item_Rarity_Normal, 85 },
			{ ObsidianGameplayTags::Item_Rarity_Magic, 10 },
			{ ObsidianGameplayTags::Item_Rarity_Rare, 5 }
		};

	const TMap<FGameplayTag, uint8> DefaultRarityToMaxSuffixCount =
	{
		{ ObsidianGameplayTags::Item_Rarity_Normal, 0 },
		{ ObsidianGameplayTags::Item_Rarity_Magic, 2 },
		{ ObsidianGameplayTags::Item_Rarity_Rare, 3 }
	};

	const TMap<FGameplayTag, uint8> DefaultRarityToMaxPrefixCount =
	{
		{ ObsidianGameplayTags::Item_Rarity_Normal, 0 },
		{ ObsidianGameplayTags::Item_Rarity_Magic, 2 },
		{ ObsidianGameplayTags::Item_Rarity_Rare, 3 }
	};

	const TMap<FGameplayTag, uint8> DefaultRarityToMaxAffixCount =
	{
		{ ObsidianGameplayTags::Item_Rarity_Normal, 0 },
		{ ObsidianGameplayTags::Item_Rarity_Magic, 2 },
		{ ObsidianGameplayTags::Item_Rarity_Rare, 6 }
	};

	const TMap<FGameplayTag, uint8> DefaultRarityToNaturalMinAffixCount =
	{
		{ ObsidianGameplayTags::Item_Rarity_Normal, 0 },
		{ ObsidianGameplayTags::Item_Rarity_Magic, 1 },
		{ ObsidianGameplayTags::Item_Rarity_Rare, 3 }
	};

	const uint8 DefaultMaxImplicitCount = 1;
}

// ~ FObsidianItemGeneratedData

void FObsidianItemGeneratedData::Reset()
{
	StackCount = 1;
	ItemRarityTag = ObsidianGameplayTags::Item_Rarity_Normal;
	ItemAffixes.Reset();
}

// ~ FDraggedItem

FDraggedItem::FDraggedItem(UObsidianInventoryItemInstance* InInstance)
	: Instance(InInstance)
	, GeneratedData(InInstance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current))
{}

// ~ FObsidianSlotDefinition

FObsidianSlotDefinition const FObsidianSlotDefinition::InvalidSlot;

bool FObsidianSlotDefinition::IsValid() const
{
	return SlotTag.IsValid();
}

bool FObsidianSlotDefinition::HasLimitedStacks() const
{
	return SlotStackLimit != INDEX_NONE;
}

FGameplayTag FObsidianSlotDefinition::GetSlotTag() const
{
	return SlotTag;
}

EObsidianEquipCheckResult FObsidianSlotDefinition::CanPlaceAtSlot(const FGameplayTag& ItemCategory) const
{
	if(BannedItemCategories.HasTagExact(ItemCategory))
	{
		return EObsidianEquipCheckResult::UnableToEquip_BannedCategory;
	}
	
	if(AcceptedItemCategories.HasTagExact(ItemCategory))
	{
		return EObsidianEquipCheckResult::CanEquip;
	}
	
	return EObsidianEquipCheckResult::ItemUnfitForCategory;
}

void FObsidianSlotDefinition::AddBannedItemCategory(const FGameplayTag& InBannedCategory)
{
	BannedItemCategories.AddTag(InBannedCategory);
}

void FObsidianSlotDefinition::AddBannedItemCategories(const FGameplayTagContainer& InBannedCategories)
{
	BannedItemCategories.AppendTags(InBannedCategories);
}

void FObsidianSlotDefinition::RemoveBannedItemCategory(const FGameplayTag& BannedCategoryToRemove)
{
#if !UE_BUILD_SHIPPING
	if(BannedItemCategories.HasTag(BannedCategoryToRemove) == false)
	{
		UE_LOG(LogTemp, Error, TEXT("Trying to remove Banned Equipment Tag [%s] but the Tag does not exist in BannedItemCategories."), *BannedCategoryToRemove.ToString());
	}
#endif
	BannedItemCategories.RemoveTag(BannedCategoryToRemove);
}

void FObsidianSlotDefinition::RemoveBannedItemCategories(const FGameplayTagContainer& BannedCategoriesToRemove)
{
#if !UE_BUILD_SHIPPING
	for(FGameplayTag Tag : BannedItemCategories)
	{
		if(BannedItemCategories.HasTag(Tag) == false)
		{
			UE_LOG(LogTemp, Error, TEXT("Trying to remove Banned Equipment Tag [%s] but the Tag does not exist in BannedItemCategories."), *Tag.ToString());
		}
	}
#endif
	BannedItemCategories.RemoveTags(BannedCategoriesToRemove);
}

// ~ FObsidianItemPosition

bool FObsidianItemPosition::IsPositionedOnGrid() const
{
	return (GridLocation != FIntPoint::NoneValue) && (SlotTag == FGameplayTag::EmptyTag);
}

bool FObsidianItemPosition::IsPositionedAtSlot() const
{
	return (GridLocation == FIntPoint::NoneValue) && (SlotTag != FGameplayTag::EmptyTag);
}

FIntPoint FObsidianItemPosition::GetItemGridLocation(const bool bWarnIfNotFound) const
{
#if !UE_BUILD_SHIPPING
	if(bWarnIfNotFound && GridLocation == FIntPoint::NoneValue)
	{
		UE_LOG(LogObsidian, Error, TEXT("Grid Location is invalid in [%hs]."), ANSI_TO_TCHAR(__FUNCTION__));
	}
#endif
	return GridLocation;
}

FGameplayTag FObsidianItemPosition::GetItemSlotTag(const bool bWarnIfNotFound) const
{
#if !UE_BUILD_SHIPPING
	if(bWarnIfNotFound && SlotTag == FGameplayTag::EmptyTag)
	{
		UE_LOG(LogObsidian, Error, TEXT("Slot Tag is invalid in [%hs]."), ANSI_TO_TCHAR(__FUNCTION__));
	}
#endif
	return SlotTag;
}

FGameplayTag FObsidianItemPosition::GetOwningStashTabTag() const
{
	return OwningStashTabTag;
}

void FObsidianItemPosition::SetOwningStashTab(const FGameplayTag& InOwningStashTab)
{
	OwningStashTabTag = InOwningStashTab;
}

FString FObsidianItemPosition::GetDebugStringPosition() const
{
	if (OwningStashTabTag != FGameplayTag::EmptyTag)
	{
		if (SlotTag != FGameplayTag::EmptyTag)
		{
			return FString::Printf(TEXT("Stash Tab: [%s], Slot: [%s]"), *OwningStashTabTag.GetTagName().ToString(), *SlotTag.GetTagName().ToString());
		}
		if (GridLocation != FIntPoint::NoneValue)
		{
			return FString::Printf(TEXT("Stash Tab: [%s], Grid Location: [%d, %d]"), *OwningStashTabTag.GetTagName().ToString(), GridLocation.X, GridLocation.Y);
		}
	}
	else
	{
		if (SlotTag != FGameplayTag::EmptyTag)
		{
			return FString::Printf(TEXT("Equipment Slot: [%s]"), *SlotTag.GetTagName().ToString());
		}
		if (GridLocation != FIntPoint::NoneValue)
		{
			return FString::Printf(TEXT("Inventory Grid Location: [%d, %d]"), GridLocation.X, GridLocation.Y);
		}
	}

	return FString::Printf(TEXT("Error: Position of an item is not initialized correctly!"));
}
