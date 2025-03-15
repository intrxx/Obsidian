// Copyright 2024 out of sCope team - Michał Ogiński


#include "InventoryItems/Fragments/Shards/ObsidianUsableShard_Identification.h"

#include "InventoryItems/ObsidianInventoryItemInstance.h"

void UObsidianUsableShard_Identification::OnItemUsed(UObsidianInventoryItemInstance* UsingInstance, UObsidianInventoryItemInstance* UsingOntoInstance)
{
	if(UsingOntoInstance && UsingInstance)
	{
		UsingOntoInstance->SetIdentified(true);
	}
}
