// Copyright 2026 out of sCope team - intrxx

#include "InventoryItems/Fragments/OInventoryItemFragment_Usable.h"

#include "InventoryItems/ObsidianInventoryItemInstance.h"


void UOInventoryItemFragment_Usable::OnInstancedCreated(UObsidianInventoryItemInstance* InInstance) const
{
	ensureMsgf(UsableItemType != EObsidianUsableItemType::UIT_None, TEXT("UsableItemType is not set on the Usable Item Fragment, this will lead to undefined behaviour, make sure to fill it."));
	InInstance->SetUsable(true);
	InInstance->SetUsableShard(UsableShard);
	InInstance->SetUsableItemType(UsableItemType);
}
