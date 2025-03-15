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
