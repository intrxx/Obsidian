// Copyright 2026 out of sCope - intrxx

#include "InventoryItems/Fragments/Shards/ObsidianUsableShard_OrbOfRepentance.h"

#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "Obsidian/ObsidianGameModule.h"

bool UObsidianUsableShard_OrbOfRepentance::OnItemUsed(AObsidianPlayerController* ItemOwner,
	UObsidianInventoryItemInstance* UsingInstance, UObsidianInventoryItemInstance* UsingOntoInstance)
{
	if(ItemOwner && UsingInstance && UsingOntoInstance)
	{
		bool bSuccess = false;
		if(CanUseOnItem(UsingOntoInstance))
		{
			TArray<FObsidianActiveItemAffix> ItemPrefixesAndSuffixes = UsingOntoInstance->GetAllItemPrefixesAndSuffixes();
			const int32 RandomAffixIndex = FMath::RandRange(0, ItemPrefixesAndSuffixes.Num() - 1);
			
			const FObsidianActiveItemAffix ChosenAffix = ItemPrefixesAndSuffixes[RandomAffixIndex];
			bSuccess = UsingOntoInstance->RemoveAffix(ChosenAffix.AffixTag);
		}

		if (bSuccess)
		{
			UE_LOG(LogObsidian, Warning, TEXT("Orb Of Repentance could not be used on provided [%s] Instance. "
									 "Or could not remove the Affix."),
										*GetNameSafe(UsingOntoInstance));
		}

		return bSuccess;
	}
	return false;
}

void UObsidianUsableShard_OrbOfRepentance::OnItemUsed_UIContext(const TArray<UObsidianInventoryItemInstance*>& AllItems,
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

bool UObsidianUsableShard_OrbOfRepentance::CanUseOnItem(const UObsidianInventoryItemInstance* Instance) const
{
	if (Instance == nullptr)
	{
		return false;
	}
	return Instance->IsItemIdentified() &&
			Instance->IsMagicOrRare() &&
			Instance->GetItemAddedPrefixAndSuffixCount() > 0;
}
