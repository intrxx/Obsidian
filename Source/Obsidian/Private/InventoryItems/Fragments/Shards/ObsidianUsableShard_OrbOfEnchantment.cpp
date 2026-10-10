// Copyright 2026 out of sCope team - intrxx

#include "InventoryItems/Fragments/Shards/ObsidianUsableShard_OrbOfEnchantment.h"

#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "InventoryItems/ObsidianItemsFunctionLibrary.h"
#include "Obsidian/ObsidianLogCategories.h"


bool UObsidianUsableShard_OrbOfEnchantment::OnItemUsed(AObsidianPlayerController* InItemOwner,
	UObsidianInventoryItemInstance* InUsingInstance, UObsidianInventoryItemInstance* InUsingOntoInstance)
{
	if(InItemOwner && InUsingInstance && InUsingOntoInstance)
	{
		if(CanUseOnItem(InUsingOntoInstance))
		{
			const bool bCanHaveAnotherPrefix = InUsingOntoInstance->CanAddPrefix();
			const bool bCanHaveAnotherSuffix = InUsingOntoInstance->CanAddSuffix();
			const bool bRollPrefix = bCanHaveAnotherPrefix && FMath::RandBool() ? true : !bCanHaveAnotherSuffix;
			if (bRollPrefix)
			{
				check(bCanHaveAnotherPrefix);
				if (const FObsidianDynamicItemAffix PrefixToAdd = UObsidianItemsFunctionLibrary::GetRandomPrefixForItem(
					InUsingOntoInstance))
				{
					FObsidianActiveItemAffix ActiveAffix;
					ActiveAffix.InitializeWithDynamic(PrefixToAdd, InUsingOntoInstance->GetItemLevel());
					InUsingOntoInstance->AddAffix(ActiveAffix);
					return true;
				}
			}
			else
			{
				check(bCanHaveAnotherSuffix);
				if (const FObsidianDynamicItemAffix SuffixToAdd = UObsidianItemsFunctionLibrary::GetRandomSuffixForItem(
					InUsingOntoInstance))
				{
					FObsidianActiveItemAffix ActiveAffix;
					ActiveAffix.InitializeWithDynamic(SuffixToAdd, InUsingOntoInstance->GetItemLevel());
					InUsingOntoInstance->AddAffix(ActiveAffix);
					return true;
				}
			}
		}
		else
		{
			UE_LOG(ObLogCrafting, Warning, TEXT("Orb Of Enchantment could not be used on provided [%s] Instance."),
				*GetNameSafe(InUsingOntoInstance));
		}
	}
	return false;
}

void UObsidianUsableShard_OrbOfEnchantment::OnItemUsed_UIContext(const TArray<UObsidianInventoryItemInstance*>& InAllItems,
	FObsidianItemsMatchingUsableContext& OutItemsMatchingContext)
{
	for(const UObsidianInventoryItemInstance* Instance : InAllItems)
	{
		if(CanUseOnItem(Instance))
		{
			OutItemsMatchingContext.AddMatchingItem(Instance);
		}
	}
}

bool UObsidianUsableShard_OrbOfEnchantment::CanUseOnItem(const UObsidianInventoryItemInstance* InInstance) const
{
	if (InInstance == nullptr)
	{
		return false;
	}
	return InInstance->CanHaveAffixes() &&
			InInstance->IsItemIdentified() &&
			InInstance->IsMagicOrRare() &&
			InInstance->CanAddPrefixOrSuffix();					
}
