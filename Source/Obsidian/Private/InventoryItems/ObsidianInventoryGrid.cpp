// Copyright 2024 out of sCope team - Michał Ogiński

#include "InventoryItems/ObsidianInventoryGrid.h"
#include "InventoryItems/ObsidianInventoryItemDefinition.h"
#include "InventoryItems/ObsidianInventoryItemFragment.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Obsidian/ObsidianGameplayTags.h"

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
	check(ItemDefClass != nullptr);
	check(OwnerComponent);

	const AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor);

	FObsidianInventoryEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.Instance = NewObject<UObsidianInventoryItemInstance>(OwnerComponent->GetOwner());
	NewEntry.Instance->SetItemDef(ItemDefClass);
	NewEntry.Instance->SetItemCurrentGridLocation(AvailablePosition);

	const UObsidianInventoryItemDefinition* DefaultObject = GetDefault<UObsidianInventoryItemDefinition>(ItemDefClass);
	for(const UObsidianInventoryItemFragment* Fragment : DefaultObject->ItemFragments)
	{
		if(Fragment)
		{
			Fragment->OnInstancedCreated(NewEntry.Instance);
		}
	}

	NewEntry.Instance->SetItemDebugName(DefaultObject->GetDebugName());
	NewEntry.StackCount = StackCount;
	NewEntry.GridLocation = AvailablePosition;
	UObsidianInventoryItemInstance* Item = NewEntry.Instance;
	
#if !UE_BUILD_SHIPPING
	if(GridLocationToItemMap.Contains(AvailablePosition))
	{
		FFrame::KismetExecutionMessage(*FString::Printf(TEXT("Provided Available Position [x: %f, y: %f] already"
			 "exist in the GridLocationToItemMap in FObsidianInventoryGrid::AddEntry"), AvailablePosition.X, AvailablePosition.Y), ELogVerbosity::Error);
	}
#endif

	GridLocationToItemMap.Add(AvailablePosition, Item);
	
	MarkItemDirty(NewEntry);
	
	BroadcastChangeMessage(NewEntry, /* Old Count */ 0, /* New Count */ NewEntry.StackCount, AvailablePosition);
	return Item;
}

void FObsidianInventoryGrid::AddEntry(UObsidianInventoryItemInstance* Instance, const FVector2D& AvailablePosition)
{
	check(Instance != nullptr);
	check(OwnerComponent);

#if !UE_BUILD_SHIPPING
	if(GridLocationToItemMap.Contains(AvailablePosition))
	{
		FFrame::KismetExecutionMessage(*FString::Printf(TEXT("Provided Available Position [x: %f, y: %f] already"
			 "exist in the GridLocationToItemMap in FObsidianInventoryGrid::AddEntry"), AvailablePosition.X, AvailablePosition.Y), ELogVerbosity::Error);
	}
#endif

	FObsidianInventoryEntry& NewEntry = Entries.Emplace_GetRef(Instance);
	NewEntry.GridLocation = AvailablePosition;
	NewEntry.Instance->SetItemCurrentGridLocation(AvailablePosition);
	NewEntry.StackCount = Instance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current);
	
	GridLocationToItemMap.Add(AvailablePosition, Instance);
	MarkItemDirty(NewEntry);
	
	BroadcastChangeMessage(NewEntry, /* Old Count */ 0, /* New Count */ NewEntry.StackCount, AvailablePosition);
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

	if(bSuccess)
	{
		const FVector2D CachedLocation = Instance->GetItemCurrentGridLocation();
		Instance->ResetItemCurrentGridLocation();
		
		GridLocationToItemMap.Remove(CachedLocation);

		const int32 StackCount = Instance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current);
		BroadcastChangeMessage(Instance, /* Old Count */ StackCount, /* New Count */ 0, CachedLocation);
		return;
	}
	FFrame::KismetExecutionMessage(TEXT("Provided Instance to remove is not in the Inventory List."), ELogVerbosity::Warning);
}

void FObsidianInventoryGrid::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	for(const int32 Index : RemovedIndices)
	{
		FObsidianInventoryEntry& Entry = Entries[Index];
		BroadcastChangeMessage(Entry, /* Old Count */ Entry.StackCount, /* New Count */ 0, Entry.GridLocation);
		Entry.LastObservedCount = 0;

		GridLocationToItemMap.Remove(Entry.GridLocation);
	}
}

void FObsidianInventoryGrid::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	UE_LOG(LogTemp, Warning, TEXT("PostReplicatedAdd, hi."));
	for(const int32 Index : AddedIndices)
	{
		FObsidianInventoryEntry& Entry = Entries[Index];
		BroadcastChangeMessage(Entry, /* Old Count */ 0, /* New Count */ Entry.StackCount, Entry.GridLocation);
		Entry.LastObservedCount = Entry.StackCount;

		GridLocationToItemMap.Add(Entry.GridLocation, Entry.Instance);
	}
}

void FObsidianInventoryGrid::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	for(const int32 Index : ChangedIndices)
	{
		FObsidianInventoryEntry& Entry = Entries[Index];
		check(Entry.LastObservedCount != INDEX_NONE);
		BroadcastChangeMessage(Entry, /* Old Count */ Entry.LastObservedCount, /* New Count */ Entry.StackCount, Entry.GridLocation);
		Entry.LastObservedCount = Entry.StackCount;
	}
}

void FObsidianInventoryGrid::BroadcastChangeMessage(const FObsidianInventoryEntry& Entry, const int32 OldCount, const int32 NewCount, const FVector2D& GridPosition) const
{
	FObsidianInventoryChangeMessage Message;
	Message.InventoryOwner = OwnerComponent;
	Message.ItemInstance = Entry.Instance;
	Message.NewCount = NewCount;
	Message.Delta = NewCount - OldCount;
	Message.GridItemPosition = GridPosition;

	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(OwnerComponent->GetWorld());
	MessageSubsystem.BroadcastMessage(ObsidianGameplayTags::Message_Inventory_Changed, Message);
}


