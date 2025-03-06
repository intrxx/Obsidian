// Copyright 2024 out of sCope team - Michał Ogiński

#include "InventoryItems/Fragments/OInventoryItemFragment_Equippable.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"

void UOInventoryItemFragment_Equippable::OnInstancedCreated(UObsidianInventoryItemInstance* Instance) const
{
	Instance->SetItemSkeletalMesh(SkeletalMesh);
	Instance->SetEquippable(true);
}
