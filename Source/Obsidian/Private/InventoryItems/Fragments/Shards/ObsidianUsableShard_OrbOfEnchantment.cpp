// Copyright 2026 out of sCope team - intrxx

#include "InventoryItems/Fragments/Shards/ObsidianUsableShard_OrbOfEnchantment.h"

#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "InventoryItems/ObsidianItemsFunctionLibrary.h"
#include "Obsidian/ObsidianGameModule.h"

bool UObsidianUsableShard_OrbOfEnchantment::OnItemUsed(AObsidianPlayerController* ItemOwner,
	UObsidianInventoryItemInstance* UsingInstance, UObsidianInventoryItemInstance* UsingOntoInstance)
{
	if(ItemOwner && UsingInstance && UsingOntoInstance)
	{
		if(CanUseOnItem(UsingOntoInstance))
		{
			const bool bCanHaveAnotherPrefix = UsingOntoInstance->CanAddPrefix();
			const bool bCanHaveAnotherSuffix = UsingOntoInstance->CanAddSuffix();
			const bool bRollPrefix = bCanHaveAnotherPrefix && FMath::RandBool() ? true : !bCanHaveAnotherSuffix;
			if (bRollPrefix)
			{
				check(bCanHaveAnotherPrefix);
				if (const FObsidianDynamicItemAffix PrefixToAdd = UObsidianItemsFunctionLibrary::GetRandomPrefixForItem(
					UsingOntoInstance))
				{
					FObsidianActiveItemAffix ActiveAffix;
					ActiveAffix.InitializeWithDynamic(PrefixToAdd, UsingOntoInstance->GetItemLevel());
					UsingOntoInstance->AddAffix(ActiveAffix);
					return true;
				}
			}
			else
			{
				check(bCanHaveAnotherSuffix);
				if (const FObsidianDynamicItemAffix SuffixToAdd = UObsidianItemsFunctionLibrary::GetRandomSuffixForItem(
					UsingOntoInstance))
				{
					FObsidianActiveItemAffix ActiveAffix;
					ActiveAffix.InitializeWithDynamic(SuffixToAdd, UsingOntoInstance->GetItemLevel());
					UsingOntoInstance->AddAffix(ActiveAffix);
					return true;
				}
			}
		}
		else
		{
			UE_LOG(LogObsidian, Warning, TEXT("Orb Of Enchantment could not be used on provided [%s] Instance."),
				*GetNameSafe(UsingOntoInstance));
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
	if (Instance == nullptr)
	{
		return false;
	}
	return Instance->CanHaveAffixes() &&
			Instance->IsItemIdentified() &&
			Instance->IsMagicOrRare() &&
			Instance->CanAddPrefixOrSuffix();					
}
