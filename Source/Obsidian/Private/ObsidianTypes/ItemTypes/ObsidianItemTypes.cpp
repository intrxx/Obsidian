// Copyright 2024 out of sCope team - Michał Ogiński

#include "ObsidianTypes/ItemTypes/ObsidianItemTypes.h"

// ~ Core

// ~ Project
#include "AbilitySystem/Attributes/ObsidianHeroAttributeSet.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "InventoryItems/ObsidianInventoryItemDefinition.h"
#include "Obsidian/ObsidianGameModule.h"


// ~ FObsidianItemGeneratedData

void FObsidianItemGeneratedData::Reset()
{
	AvailableStackCount = 1;
	ItemRarity = EObsidianItemRarity::None;
	ItemAffixes.Reset();
	NameData = FObsidianItemGeneratedNameData();
	ItemEquippingRequirements = FObsidianItemRequirements();
}

// ~ FDraggedItem

FDraggedItem::FDraggedItem(UObsidianInventoryItemInstance* InInstance) 
	: Instance(InInstance)
	, GeneratedData(InInstance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current)) //TODO(intrxx) do I really need stacks here?
{}

bool FDraggedItem::IsEmpty() const
{
	return !Instance && !ItemDef;
}

bool FDraggedItem::CarriesItemDef() const
{
	return ItemDef != nullptr;
}

// ~ FObsidianSlotDefinition

FObsidianSlotDefinition const FObsidianSlotDefinition::InvalidSlot;

// ~ FObsidianItemRequirement

FObsidianItemRequirements::FObsidianItemRequirements()
{
	AttributeRequirements =
		{
			{FObsidianAttributeRequirement(UObsidianHeroAttributeSet::GetStrengthAttribute(), 0)},
			{FObsidianAttributeRequirement(UObsidianHeroAttributeSet::GetIntelligenceAttribute(), 0)},
			{FObsidianAttributeRequirement(UObsidianHeroAttributeSet::GetDexterityAttribute(), 0)},
			{FObsidianAttributeRequirement(UObsidianHeroAttributeSet::GetFaithAttribute(), 0)}
		};
}

// ~ FObsidianItemRequirement

bool FObsidianSlotDefinition::IsValid() const
{
	return SlotTag.IsValid();
}

FGameplayTag FObsidianSlotDefinition::GetSlotTag() const
{
	return SlotTag;
}

EObsidianPlacingAtSlotResult FObsidianSlotDefinition::CanPlaceAtSlot(const FGameplayTag& ItemCategory) const
{
	if(BannedItemCategories.HasTagExact(ItemCategory))
	{
		return EObsidianPlacingAtSlotResult::UnableToPlace_BannedCategory;
	}
	
	if(AcceptedItemCategories.HasTagExact(ItemCategory))
	{
		return EObsidianPlacingAtSlotResult::CanPlace;
	}
	
	return EObsidianPlacingAtSlotResult::UnableToPlace_UnfitForCategory;
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
