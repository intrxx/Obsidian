// Copyright 2024 out of sCope team - Michał Ogiński

#include "InventoryItems/PlayerStash/ObsidianStashItemList.h"

// ~ Core

// ~ Project
#include "InventoryItems/ObsidianInventoryItemInstance.h"

// ~ FObsidianStashEntry

FString FObsidianStashEntry::GetDebugString() const
{
	TSubclassOf<UObsidianInventoryItemDefinition> ItemDef;
	if(Instance != nullptr)
	{
		ItemDef = Instance->GetItemDef();
	}

	return FString::Printf(TEXT("Instance: %s [%d,%s]"), *GetNameSafe(Instance), StackCount, *GetNameSafe(ItemDef));
}

// ~ End of FObsidianStashEntry

TArray<UObsidianInventoryItemInstance*> FObsidianStashItemList::GetAllItems() const
{
	TArray<UObsidianInventoryItemInstance*> Items;
	Items.Reserve(Entries.Num());

	for(const FObsidianStashEntry& Entry : Entries)
	{
		if(Entry.Instance)
		{
			Items.Add(Entry.Instance);
		}
	}
	return Items;
}

int32 FObsidianStashItemList::GetEntriesCount() const
{
	return Entries.Num();
}

UObsidianInventoryItemInstance* FObsidianStashItemList::AddEntry(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDefClass, const int32 StackCount, const FObsidianItemPosition& ToPosition)
{
	return nullptr;
}

void FObsidianStashItemList::AddEntry(UObsidianInventoryItemInstance* Instance, const FObsidianItemPosition& ToPosition)
{
}

void FObsidianStashItemList::Item_MarkSpace(const UObsidianInventoryItemInstance* ItemInstance, const FObsidianItemPosition& AtPosition)
{
}

void FObsidianStashItemList::Item_UnMarkSpace(const UObsidianInventoryItemInstance* ItemInstance, const FObsidianItemPosition& AtPosition)
{
}

void FObsidianStashItemList::RemoveEntry(UObsidianInventoryItemInstance* Instance)
{
}

void FObsidianStashItemList::ChangedEntryStacks(UObsidianInventoryItemInstance* Instance, const int32 OldCount)
{
}

void FObsidianStashItemList::GeneralEntryChange(UObsidianInventoryItemInstance* Instance)
{
}

void FObsidianStashItemList::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
}

void FObsidianStashItemList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
}

void FObsidianStashItemList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
}

void FObsidianStashItemList::BroadcastChangeMessage(const FObsidianStashEntry& Entry, const int32 OldCount, const int32 NewCount, const FObsidianItemPosition& ItemPosition, const EObsidianStashChangeType& ChangeType) const
{
}

