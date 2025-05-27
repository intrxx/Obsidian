// Copyright 2024 out of sCope team - Michał Ogiński

#include "InventoryItems/Fragments/OInventoryItemFragment_Usable.h"

// ~ Core

// ~ Project
#include "InventoryItems/ObsidianInventoryItemInstance.h"

void UOInventoryItemFragment_Usable::OnInstancedCreated(UObsidianInventoryItemInstance* Instance) const
{
	Instance->SetUsable(true);
	Instance->SetUsableShard(UsableShard);
}
