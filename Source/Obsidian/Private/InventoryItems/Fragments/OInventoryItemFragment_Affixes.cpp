// Copyright 2024 out of sCope team - Michał Ogiński

#include "InventoryItems/Fragments/OInventoryItemFragment_Affixes.h"

#include "InventoryItems/ObsidianInventoryItemInstance.h"

void UOInventoryItemFragment_Affixes::OnInstancedCreated(UObsidianInventoryItemInstance* Instance) const
{
	Instance->SetItemRarity(ItemRarityTag);
}
