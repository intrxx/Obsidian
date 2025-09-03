// Copyright 2024 out of sCope team - Michał Ogiński

#include "InventoryItems/Fragments/Shards/ObsidianUsableShard_Identification.h"

// ~ Core

// ~ Project
#include "InventoryItems/ObsidianInventoryItemInstance.h"

bool UObsidianUsableShard_Identification::OnItemUsed(AObsidianPlayerController* ItemOwner, UObsidianInventoryItemInstance* UsingInstance, UObsidianInventoryItemInstance* UsingOntoInstance)
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

FObsidianItemsMatchingUsableContext UObsidianUsableShard_Identification::OnItemUsed_UIContext(const TArray<UObsidianInventoryItemInstance*>& AllItems)
{
	FObsidianItemsMatchingUsableContext ItemsMatchingContext;
	
	for(const UObsidianInventoryItemInstance* Instance : AllItems)
	{
		if(Instance->IsItemIdentified() == false)
		{
			const FObsidianItemPosition CurrentPosition = Instance->GetItemCurrentPosition();

			if (CurrentPosition.GetOwningStashTabTag() != FGameplayTag::EmptyTag)
			{
				ItemsMatchingContext.StashItemsMatchingContext.Add(CurrentPosition);
				continue;
			}
			
			const FIntPoint GridLocation = CurrentPosition.GetItemGridLocation();
			if(GridLocation != FIntPoint::NoneValue)
			{
				ItemsMatchingContext.InventoryItemsMatchingContext.Add(GridLocation);
				continue;
			}
		}
	}
	
	return ItemsMatchingContext;
}
