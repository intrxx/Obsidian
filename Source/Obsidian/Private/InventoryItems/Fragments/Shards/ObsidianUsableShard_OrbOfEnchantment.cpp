// Copyright 2026 out of sCope team - intrxx

#include "InventoryItems/Fragments/Shards/ObsidianUsableShard_OrbOfEnchantment.h"

// ~ Core

// ~ Project
#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "InventoryItems/ObsidianItemsFunctionLibrary.h"

bool UObsidianUsableShard_OrbOfEnchantment::OnItemUsed(AObsidianPlayerController* ItemOwner,
	UObsidianInventoryItemInstance* UsingInstance, UObsidianInventoryItemInstance* UsingOntoInstance)
{
	if(ItemOwner && UsingInstance && UsingOntoInstance)
	{
		if(CanUseOnItem(UsingOntoInstance))
		{
			//TODO(intrxx) #AffixRefactor
			// FObsidianDynamicItemAffix TempItemAffixToAdd;
			// TempItemAffixToAdd.AffixTag = ObsidianGameplayTags::Item_Affix_Enchant_MaximumLifePercentage;
			// TempItemAffixToAdd.AffixTierValue = 2;
			// TempItemAffixToAdd.AffixType = EObsidianAffixType::Prefix;
			// TempItemAffixToAdd.TempAffixMagnitude = 8;
			// TempItemAffixToAdd.AffixDescription = FText::FromString(TEXT("To Maximum Life"));
			//
			// UsingOntoInstance->AddAffix(TempItemAffixToAdd);

			UE_LOG(LogTemp, Error, TEXT("Orb Of Enchantment needs implementing."));
			return true;
		}
	}
	return false;
}

void UObsidianUsableShard_OrbOfEnchantment::OnItemUsed_UIContext(const TArray<UObsidianInventoryItemInstance*>& AllItems,
	FObsidianItemsMatchingUsableContext& OutItemsMatchingContext)
{
	for(const UObsidianInventoryItemInstance* Instance : AllItems)
	{
		if(CanUseOnItem(Instance))
		{
			OutItemsMatchingContext.AddMatchingItem(Instance);
		}
	}
}

bool UObsidianUsableShard_OrbOfEnchantment::CanUseOnItem(const UObsidianInventoryItemInstance* Instance) const
{
	const EObsidianItemRarity ItemRarity = Instance->GetItemRarity();
	const bool bUsableRarity = ItemRarity > EObsidianItemRarity::Normal && ItemRarity < EObsidianItemRarity::Unique;
	return	bUsableRarity && Instance->IsItemEquippable() && Instance->IsItemIdentified() &&
				Instance->GetItemAddedAffixCount() < Instance->GetItemCombinedAffixLimit();					
}
