// Copyright 2024 out of sCope team - Michał Ogiński


#include "InventoryItems/Fragments/Shards/ObsidianUsableShard_Identification.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"


bool UObsidianUsableShard_Identification::OnItemUsed(UObsidianInventoryItemInstance* UsingInstance, UObsidianInventoryItemInstance* UsingOntoInstance)
{
	if(UsingOntoInstance && UsingInstance)
	{
		if(UsingOntoInstance->IsItemIdentified() == false)
		{
			UsingOntoInstance->SetIdentified(true);
			return true;
		}
	}
	return false;
}

FObsidianItemsMatchingUsableContext UObsidianUsableShard_Identification::OnItemUsed_UIContext(const TArray<UObsidianInventoryItemInstance*>& AllItems)
{
	FObsidianItemsMatchingUsableContext ItemsMatchingContext;
	
	for(const UObsidianInventoryItemInstance* Instance : AllItems)
	{
		if(Instance->IsItemIdentified() == false)
		{
			const FVector2D GridLocation = Instance->GetItemCurrentGridLocation();
			if(GridLocation != FVector2D::ZeroVector)
			{
				ItemsMatchingContext.InventoryItemsMatchingContext.Add(GridLocation);
				continue;
			}
			
			//TODO Add Stash when implemented
			
		}
	}
	
	return ItemsMatchingContext;
}
