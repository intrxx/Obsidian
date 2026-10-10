// Copyright 2026 out of sCope - intrxx

#include "InventoryItems/Fragments/Shards/ObsidianUsableShard_OrbOfEradication.h"

#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "Obsidian/ObsidianLogCategories.h"


bool UObsidianUsableShard_OrbOfEradication::OnItemUsed(AObsidianPlayerController* InItemOwner,
	UObsidianInventoryItemInstance* InUsingInstance, UObsidianInventoryItemInstance* InUsingOntoInstance)
{
	if(InItemOwner && InUsingInstance && InUsingOntoInstance)
	{
		bool bSuccess = false;
		if(CanUseOnItem(InUsingOntoInstance))
		{
			if (InUsingOntoInstance->RemoveAllPrefixesAndSuffixes())
			{
				InUsingOntoInstance->SetItemRarity(EObsidianItemRarity::Normal);
				bSuccess = true;
			}
		}
		
		if (bSuccess == false)
		{
			UE_LOG(ObLogCrafting, Warning, TEXT("Orb Of Eradication could not be used on provided [%s] Instance."
									 "Or the Usage failed to Remove any Prefixes or Suffixes."),
										*GetNameSafe(InUsingOntoInstance));
		}

		return bSuccess;
	}
	return false;
}

void UObsidianUsableShard_OrbOfEradication::OnItemUsed_UIContext(const TArray<UObsidianInventoryItemInstance*>& InAllItems,
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

bool UObsidianUsableShard_OrbOfEradication::CanUseOnItem(const UObsidianInventoryItemInstance* InInstance) const
{
	if (InInstance == nullptr)
	{
		return false;
	}
	return InInstance->IsItemIdentified() &&
			InInstance->IsMagicOrRare() &&
			InInstance->GetItemAddedPrefixAndSuffixCount() > 0;
}
