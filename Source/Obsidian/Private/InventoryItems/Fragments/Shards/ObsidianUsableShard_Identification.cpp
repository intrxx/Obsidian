// Copyright 2026 out of sCope team - intrxx

#include "InventoryItems/Fragments/Shards/ObsidianUsableShard_Identification.h"


#include "InventoryItems/ObsidianInventoryItemInstance.h"

bool UObsidianUsableShard_Identification::OnItemUsed(AObsidianPlayerController* ItemOwner,
	UObsidianInventoryItemInstance* UsingInstance, UObsidianInventoryItemInstance* UsingOntoInstance)
{
	if(ItemOwner && UsingOntoInstance && UsingInstance)
	{
		if(UsingOntoInstance->IsItemIdentified() == false)
		{
			UsingOntoInstance->SetIdentified(true);
			return true;
		}
	}
	return false;
}

void UObsidianUsableShard_Identification::OnItemUsed_UIContext(const TArray<UObsidianInventoryItemInstance*>& AllItems,
	FObsidianItemsMatchingUsableContext& OutItemsMatchingContext)
{
	for(const UObsidianInventoryItemInstance* Instance : AllItems)
	{
		if(Instance->IsItemIdentified() == false)
		{
			OutItemsMatchingContext.AddMatchingItem(Instance);
		}
	}
}
