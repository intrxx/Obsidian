// Copyright 2026 out of sCope - intrxx

#include "InventoryItems/Fragments/Shards/ObsidianUsableShard_OrbOfEradication.h"

#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "Obsidian/ObsidianGameModule.h"

bool UObsidianUsableShard_OrbOfEradication::OnItemUsed(AObsidianPlayerController* ItemOwner,
	UObsidianInventoryItemInstance* UsingInstance, UObsidianInventoryItemInstance* UsingOntoInstance)
{
	if(ItemOwner && UsingInstance && UsingOntoInstance)
	{
		bool bSuccess = false;
		if(CanUseOnItem(UsingOntoInstance))
		{
			if (UsingOntoInstance->RemoveAllPrefixesAndSuffixes())
			{
				UsingOntoInstance->SetItemRarity(EObsidianItemRarity::Normal);
				bSuccess = true;
			}
		}
		
		if (bSuccess == false)
		{
			UE_LOG(LogObsidian, Warning, TEXT("Orb Of Eradication could not be used on provided [%s] Instance."
									 "Or the Usage failed to Remove any Prefixes or Suffixes."),
										*GetNameSafe(UsingOntoInstance));
		}

		return bSuccess;
	}
	return false;
}

void UObsidianUsableShard_OrbOfEradication::OnItemUsed_UIContext(const TArray<UObsidianInventoryItemInstance*>& AllItems,
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

bool UObsidianUsableShard_OrbOfEradication::CanUseOnItem(const UObsidianInventoryItemInstance* Instance) const
{
	if (Instance == nullptr)
	{
		return false;
	}
	return Instance->IsItemIdentified() &&
			Instance->IsMagicOrRare() &&
			Instance->GetItemAddedPrefixAndSuffixCount() > 0;
}
