// Copyright 2024 out of sCope team - Michał Ogiński

#include "ObsidianTypes/ObsidianItemTypes.h"

// ~ Core

// ~ Project
#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "InventoryItems/Fragments/OInventoryItemFragment_Affixes.h"
#include "Obsidian/ObsidianGameModule.h"

// ~ FDraggedItem

FDraggedItem::FDraggedItem(UObsidianInventoryItemInstance* InInstance)
	: Instance(InInstance)
	, Stacks(InInstance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current))
{}

// ~ FObsidianSlotDefinition

FObsidianSlotDefinition const FObsidianSlotDefinition::InvalidSlot;

bool FObsidianSlotDefinition::IsValid() const
{
	return SlotTag.IsValid();
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

bool FObsidianItemPosition::IsPositionedOnGrid() const
{
	return (GridLocation != FIntPoint::NoneValue) && (SlotTag == FGameplayTag::EmptyTag);
}

bool FObsidianItemPosition::IsPositionedAtSlot() const
{
	return (GridLocation == FIntPoint::NoneValue) && (SlotTag != FGameplayTag::EmptyTag);
}

// ~ FObsidianItemPosition

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

// ~ FObsidianDescriptionAffixRow

void FObsidianAffixDescriptionRow::SetAffixRowDescription(const FText& InAffixDescription, const int32 InTempMagnitude)
{
	AffixRowDescription = FText::FromString(FString::Printf(TEXT("Adds %d %s"), InTempMagnitude, *InAffixDescription.ToString()));
}

void FObsidianAffixDescriptionRow::SetAffixAdditionalDescription(const EObsidianAffixType& InAffixType, const int32 InAffixTier)
{
	AffixType = InAffixType;
	FText AffixTypeText = FText();
	
	switch (InAffixType)
	{
	case EObsidianAffixType::Implicit:
		AffixTypeText = FText::FromString(FString::Printf(TEXT("Implicit, ")));
		break;
	case EObsidianAffixType::Prefix:
		AffixTypeText = FText::FromString(FString::Printf(TEXT("Prefix, ")));
		break;
	case EObsidianAffixType::Suffix:
		AffixTypeText = FText::FromString(FString::Printf(TEXT("Suffix, ")));
		break;
		default:
			AffixTypeText = FText::FromString(FString::Printf(TEXT("Affix, ")));
			break;
	}
	
	AffixAdditionalDescription = FText::FromString(FString::Printf(TEXT("%s tier: %d "), *AffixTypeText.ToString(), InAffixTier));
}

// ~ End of FObsidianDescriptionAffixRow
