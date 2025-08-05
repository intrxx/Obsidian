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
			FObsidianItemAffix TempItemAffixToAdd;
			TempItemAffixToAdd.AffixTag = ObsidianGameplayTags::Item_Affix_Enchant_MaximumLifePercentage;
			TempItemAffixToAdd.AffixTier = 2;
			TempItemAffixToAdd.AffixType = EObsidianAffixType::Prefix;
			TempItemAffixToAdd.TempAffixMagnitude = 8;
			TempItemAffixToAdd.AffixDescription = FText::FromString(TEXT("To Maximum Life"));
			
			UsingOntoInstance->AddAffix(TempItemAffixToAdd);
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
			const FIntPoint GridLocation = Instance->GetItemCurrentGridLocation();
			if(GridLocation != FIntPoint::NoneValue)
			{
				ItemsMatchingContext.InventoryItemsMatchingContext.Add(GridLocation);
				continue;
			}
			
			//TODO Add Stash when implemented
		}
	}
	
	return ItemsMatchingContext;
}

bool UObsidianUsableShard_OrbOfEnchantment::CanUseOnItem(const UObsidianInventoryItemInstance* Instance) const
{
	return Instance->IsItemEquippable() &&
				Instance->IsItemIdentified() &&
				UObsidianItemsFunctionLibrary::IsItemUnique(Instance) == false &&
				Instance->GetItemAddedAffixCount() < Instance->GetItemCombinedAffixLimit();				
}
