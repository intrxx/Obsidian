// Copyright 2024 out of sCope team - Michał Ogiński

#include "InventoryItems/ObsidianInventoryGrid.h"
#include "InventoryItems/ObsidianInventoryItemDefinition.h"
#include "InventoryItems/ObsidianInventoryItemFragment.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"

// ---- Start of FObsidianInventoryEntry ----

FString FObsidianInventoryEntry::GetDebugString() const
{
	TSubclassOf<UObsidianInventoryItemDefinition> ItemDef;
	if(Instance != nullptr)
	{
		ItemDef = Instance->GetItemDef();
	}

	return FString::Printf(TEXT("Instance: %s [%d,%s]"), *GetNameSafe(Instance), StackCount, *GetNameSafe(ItemDef));
}

// ---- End of FObsidianInventoryEntry ----

TArray<UObsidianInventoryItemInstance*> FObsidianInventoryGrid::GetAllItems() const
{
	TArray<UObsidianInventoryItemInstance*> Items;
	Items.Reserve(Entries.Num());

	for(const FObsidianInventoryEntry& Entry : Entries)
	{
		if(Entry.Instance)
		{
			Items.Add(Entry.Instance);
		}
	}
	return Items;
}

int32 FObsidianInventoryGrid::GetEntriesCount() const
{
	return Entries.Num();
}

UObsidianInventoryItemInstance* FObsidianInventoryGrid::AddEntry(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDefClass, const int32 StackCount, const FVector2D& AvailablePosition)
{
	UObsidianInventoryItemInstance* Item = nullptr;

	check(ItemDefClass != nullptr);
	check(OwnerComponent);

	const AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor);

	FObsidianInventoryEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.Instance = NewObject<UObsidianInventoryItemInstance>(OwnerComponent->GetOwner());
	NewEntry.Instance->SetItemDef(ItemDefClass);

	for(const UObsidianInventoryItemFragment* Fragment : GetDefault<UObsidianInventoryItemDefinition>(ItemDefClass)->ItemFragments)
	{
		if(Fragment)
		{
			Fragment->OnInstancedCreated(NewEntry.Instance);
		}
	}

	NewEntry.StackCount = StackCount;
	Item = NewEntry.Instance;

	GridLocationToItemMap.Add(AvailablePosition, Item);

	MarkItemDirty(NewEntry);

	return Item;
}

void FObsidianInventoryGrid::AddEntry(UObsidianInventoryItemInstance* Instance)
{
	//TODO Implement when grabbing item instances
}

void FObsidianInventoryGrid::RemoveEntry(UObsidianInventoryItemInstance* Instance)
{
	bool bSuccess = false;
	for(auto It = Entries.CreateIterator(); It; ++It)
	{
		FObsidianInventoryEntry& Entry = *It;
		if(Entry.Instance == Instance)
		{
			It.RemoveCurrent();
			MarkArrayDirty();
			bSuccess = true;
		}
	}

	if(!bSuccess)
	{
		FFrame::KismetExecutionMessage(TEXT("Provided Instance to remove is not in the Inventory List."), ELogVerbosity::Warning);
	}
}

void FObsidianInventoryGrid::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	for(const int32 Index : RemovedIndices)
	{
		FObsidianInventoryEntry& Entry = Entries[Index];
		Entry.LastObservedCount = 0;
	}
}

void FObsidianInventoryGrid::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for(const int32 Index : AddedIndices)
	{
		FObsidianInventoryEntry& Entry = Entries[Index];
		Entry.LastObservedCount = Entry.StackCount;
	}
}

void FObsidianInventoryGrid::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	for(const int32 Index : ChangedIndices)
	{
		FObsidianInventoryEntry& Entry = Entries[Index];
		check(Entry.LastObservedCount != INDEX_NONE);
		Entry.LastObservedCount = Entry.StackCount;
	}
}


