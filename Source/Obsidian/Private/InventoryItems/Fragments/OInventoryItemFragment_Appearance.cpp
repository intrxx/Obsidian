// Copyright 2024 out of sCope team - Michał Ogiński


#include "InventoryItems/Fragments/OInventoryItemFragment_Appearance.h"

#include "InventoryItems/ObsidianInventoryItemInstance.h"

void UOInventoryItemFragment_Appearance::OnInstancedCreated(UObsidianInventoryItemInstance* Instance) const
{
	if(Instance)
	{
		Instance->SetItemImage(ItemImage);
	}
}
