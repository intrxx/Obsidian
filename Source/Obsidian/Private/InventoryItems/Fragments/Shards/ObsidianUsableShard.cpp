// Copyright 2024 out of sCope team - Michał Ogiński

#include "InventoryItems/Fragments/Shards/ObsidianUsableShard.h"

// ~ Core

// ~ Project

bool UObsidianUsableShard::OnItemUsed(UObsidianInventoryItemInstance* UsingInstance, UObsidianInventoryItemInstance* UsingOntoInstance)
{
	return false;
}

FObsidianItemsMatchingUsableContext UObsidianUsableShard::OnItemUsed_UIContext(const TArray<UObsidianInventoryItemInstance*>& AllItems)
{
	return FObsidianItemsMatchingUsableContext();
}
