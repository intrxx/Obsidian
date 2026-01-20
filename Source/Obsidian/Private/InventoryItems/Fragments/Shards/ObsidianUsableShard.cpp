// Copyright 2026 out of sCope team - intrxx

#include "InventoryItems/Fragments/Shards/ObsidianUsableShard.h"

// ~ Core

// ~ Project

bool UObsidianUsableShard::OnItemUsed(AObsidianPlayerController* ItemOwner, UObsidianInventoryItemInstance* UsingInstance, UObsidianInventoryItemInstance* UsingOntoInstance)
{
	return false;
}

FObsidianItemsMatchingUsableContext UObsidianUsableShard::OnItemUsed_UIContext(const TArray<UObsidianInventoryItemInstance*>& AllItems)
{
	return FObsidianItemsMatchingUsableContext();
}
