// Copyright 2024 out of sCope team - Michał Ogiński

#include "InventoryItems/Equipment/ObsidianEquipmentList.h"

UObsidianInventoryItemInstance* FObsidianEquipmentList::AddEntry(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDefClass, const FVector2D& EquipmentSlotTag)
{
	return nullptr;
}

void FObsidianEquipmentList::AddEntry(UObsidianInventoryItemInstance* Instance, const FGameplayTag& EquipmentSlotTag)
{
}

void FObsidianEquipmentList::RemoveEntry(UObsidianInventoryItemInstance* Instance)
{
}

void FObsidianEquipmentList::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
}

void FObsidianEquipmentList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
}

void FObsidianEquipmentList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
}
