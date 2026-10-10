// Copyright 2026 out of sCope - intrxx

#include "InventoryItems/Fragments/Shards/ObsidianUsableShard_OrbOfRepentance.h"

#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "Obsidian/ObsidianLogCategories.h"


bool UObsidianUsableShard_OrbOfRepentance::OnItemUsed(AObsidianPlayerController* InItemOwner,
	UObsidianInventoryItemInstance* InUsingInstance, UObsidianInventoryItemInstance* InUsingOntoInstance)
{
	if(InItemOwner && InUsingInstance && InUsingOntoInstance)
	{
		bool bSuccess = false;
		if(CanUseOnItem(InUsingOntoInstance))
		{
			TArray<FObsidianActiveItemAffix> ItemPrefixesAndSuffixes = InUsingOntoInstance->GetAllItemPrefixesAndSuffixes();
			const int32 RandomAffixIndex = FMath::RandRange(0, ItemPrefixesAndSuffixes.Num() - 1);
			
			const FObsidianActiveItemAffix ChosenAffix = ItemPrefixesAndSuffixes[RandomAffixIndex];
			bSuccess = InUsingOntoInstance->RemoveAffix(ChosenAffix.AffixTag);
		}

		if (bSuccess)
		{
			UE_LOG(ObLogCrafting, Warning, TEXT("Orb Of Repentance could not be used on provided [%s] Instance. "
									 "Or could not remove the Affix."),
										*GetNameSafe(InUsingOntoInstance));
		}

		return bSuccess;
	}
	return false;
}

void UObsidianUsableShard_OrbOfRepentance::OnItemUsed_UIContext(const TArray<UObsidianInventoryItemInstance*>& InAllItems,
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

bool UObsidianUsableShard_OrbOfRepentance::CanUseOnItem(const UObsidianInventoryItemInstance* InInstance) const
{
	if (InInstance == nullptr)
	{
		return false;
	}
	return InInstance->IsItemIdentified() &&
			InInstance->IsMagicOrRare() &&
			InInstance->GetItemAddedPrefixAndSuffixCount() > 0;
}
