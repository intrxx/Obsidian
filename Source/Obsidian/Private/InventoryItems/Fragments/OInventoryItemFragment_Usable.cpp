// Copyright 2024 out of sCope team - Michał Ogiński

#include "InventoryItems/Fragments/OInventoryItemFragment_Usable.h"

// ~ Core

// ~ Project
#include "InventoryItems/ObsidianInventoryItemInstance.h"

void UOInventoryItemFragment_Usable::OnInstancedCreated(UObsidianInventoryItemInstance* Instance) const
{
	ensureMsgf(UsableItemType != EObsidianUsableItemType::UIT_None, TEXT("UsableItemType is not set on the Usable Item Fragment, this will lead to undefined behaviour, make sure to fill it."));
	Instance->SetUsable(true);
	Instance->SetUsableShard(UsableShard);
	Instance->SetUsableItemType(UsableItemType);
}
