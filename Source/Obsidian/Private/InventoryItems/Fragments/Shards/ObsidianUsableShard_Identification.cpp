// Copyright 2026 out of sCope team - intrxx

#include "InventoryItems/Fragments/Shards/ObsidianUsableShard_Identification.h"

#include "InventoryItems/ObsidianInventoryItemInstance.h"


bool UObsidianUsableShard_Identification::OnItemUsed(AObsidianPlayerController* InItemOwner,
	UObsidianInventoryItemInstance* InUsingInstance, UObsidianInventoryItemInstance* InUsingOntoInstance)
{
	if(InItemOwner && InUsingOntoInstance && InUsingInstance)
	{
		if(InUsingOntoInstance->IsItemIdentified() == false)
		{
			InUsingOntoInstance->SetIdentified(true);
			return true;
		}
	}
	return false;
}

void UObsidianUsableShard_Identification::OnItemUsed_UIContext(const TArray<UObsidianInventoryItemInstance*>& InAllItems,
	FObsidianItemsMatchingUsableContext& OutItemsMatchingContext)
{
	for(const UObsidianInventoryItemInstance* Instance : InAllItems)
	{
		if(Instance->IsItemIdentified() == false)
		{
			OutItemsMatchingContext.AddMatchingItem(Instance);
		}
	}
}
