// Copyright 2024 out of sCope team - Michał Ogiński

#include "InventoryItems/Fragments/Shards/ObsidianUsableShard_OrbOfEnchantment.h"

// ~ Core

// ~ Project
#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "InventoryItems/ObsidianItemsFunctionLibrary.h"

bool UObsidianUsableShard_OrbOfEnchantment::OnItemUsed(AObsidianPlayerController* ItemOwner, UObsidianInventoryItemInstance* UsingInstance, UObsidianInventoryItemInstance* UsingOntoInstance)
{
	if(ItemOwner && UsingInstance && UsingOntoInstance)
	{
		if(CanUseOnItem(UsingOntoInstance))
		{
			//TODO(intrxx) #AffixRefactor
			// FObsidianDynamicItemAffix TempItemAffixToAdd;
			// TempItemAffixToAdd.AffixTag = ObsidianGameplayTags::Item_Affix_Enchant_MaximumLifePercentage;
			// TempItemAffixToAdd.AffixTier = 2;
			// TempItemAffixToAdd.AffixType = EObsidianAffixType::Prefix;
			// TempItemAffixToAdd.TempAffixMagnitude = 8;
			// TempItemAffixToAdd.AffixDescription = FText::FromString(TEXT("To Maximum Life"));
			//
			// UsingOntoInstance->AddAffix(TempItemAffixToAdd);
			return true;
		}
	}
	return false;
}

FObsidianItemsMatchingUsableContext UObsidianUsableShard_OrbOfEnchantment::OnItemUsed_UIContext(const TArray<UObsidianInventoryItemInstance*>& AllItems)
{
	FObsidianItemsMatchingUsableContext ItemsMatchingContext;
	
	for(const UObsidianInventoryItemInstance* Instance : AllItems)
	{
		if(CanUseOnItem(Instance))
		{
			const FObsidianItemPosition CurrentPosition = Instance->GetItemCurrentPosition();

			if (CurrentPosition.GetOwningStashTabTag() != FGameplayTag::EmptyTag)
			{
				ItemsMatchingContext.StashItemsMatchingContext.Add(CurrentPosition);
				continue;
			}
			
			const FIntPoint GridLocation = CurrentPosition.GetItemGridLocation();
			if(GridLocation != FIntPoint::NoneValue)
			{
				ItemsMatchingContext.InventoryItemsMatchingContext.Add(GridLocation);
				continue;
			}
		}
	}
	
	return ItemsMatchingContext;
}

bool UObsidianUsableShard_OrbOfEnchantment::CanUseOnItem(const UObsidianInventoryItemInstance* Instance) const
{
	const EObsidianItemRarity ItemRarity = Instance->GetItemRarity();
	const bool bUsableRarity = ItemRarity > EObsidianItemRarity::Normal && ItemRarity < EObsidianItemRarity::Unique;
	return	bUsableRarity && Instance->IsItemEquippable() && Instance->IsItemIdentified() &&
				Instance->GetItemAddedAffixCount() < Instance->GetItemCombinedAffixLimit();					
}
