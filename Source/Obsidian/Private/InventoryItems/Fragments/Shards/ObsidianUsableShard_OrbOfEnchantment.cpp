// Copyright 2024 out of sCope team - Michał Ogiński

#include "InventoryItems/Fragments/Shards/ObsidianUsableShard_OrbOfEnchantment.h"

// ~ Core

// ~ Project
#include "InventoryItems/ObsidianInventoryItemInstance.h"

bool UObsidianUsableShard_OrbOfEnchantment::OnItemUsed(AObsidianPlayerController* ItemOwner, UObsidianInventoryItemInstance* UsingInstance, UObsidianInventoryItemInstance* UsingOntoInstance)
{
	if(ItemOwner && UsingInstance && UsingOntoInstance)
	{
		
		return true;
	}
	return false;
}

FObsidianItemsMatchingUsableContext UObsidianUsableShard_OrbOfEnchantment::OnItemUsed_UIContext(const TArray<UObsidianInventoryItemInstance*>& AllItems)
{
	FObsidianItemsMatchingUsableContext ItemsMatchingContext;
	
	for(const UObsidianInventoryItemInstance* Instance : AllItems)
	{
		// if(Instance->IsItemIdentified() == false)
		// {
		// 	const FIntPoint GridLocation = Instance->GetItemCurrentGridLocation();
		// 	if(GridLocation != FIntPoint::NoneValue)
		// 	{
		// 		ItemsMatchingContext.InventoryItemsMatchingContext.Add(GridLocation);
		// 		continue;
		// 	}
		// 	
		// 	//TODO Add Stash when implemented
		// 	
		// }
	}
	
	return ItemsMatchingContext;
}
