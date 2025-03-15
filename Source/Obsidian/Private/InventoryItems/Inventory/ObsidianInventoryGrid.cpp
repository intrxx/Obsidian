// Copyright 2024 out of sCope team - Michał Ogiński

#include "InventoryItems/Inventory/ObsidianInventoryGrid.h"
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
	
	NewEntry.Instance->SetItemCategory(DefaultObject->GetItemCategoryTag());
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
	Item_MarkSpace(Item, AvailablePosition);
	
	MarkItemDirty(NewEntry);
	
	BroadcastChangeMessage(NewEntry, /* Old Count */ 0, /* New Count */ NewEntry.StackCount, AvailablePosition, EObsidianInventoryChangeType::ICT_ItemAdded);
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
	NewEntry.GridLocation = AvailablePosition; //TODO Add Grid Location to Entry instead of instance?
	NewEntry.Instance->SetItemCurrentGridLocation(AvailablePosition);
	NewEntry.StackCount = Instance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current);
	
	GridLocationToItemMap.Add(AvailablePosition, Instance);
	Item_MarkSpace(Instance, AvailablePosition);
	MarkItemDirty(NewEntry);
	
	BroadcastChangeMessage(NewEntry, /* Old Count */ 0, /* New Count */ NewEntry.StackCount, AvailablePosition, EObsidianInventoryChangeType::ICT_ItemAdded);
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
		Item_UnMarkSpace(Instance, CachedLocation);

		const int32 StackCount = Instance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current);
		BroadcastChangeMessage(Instance, /* Old Count */ StackCount, /* New Count */ 0, CachedLocation, EObsidianInventoryChangeType::ICT_ItemRemoved);
		return;
	}
	FFrame::KismetExecutionMessage(TEXT("Provided Instance to remove is not in the Inventory List."), ELogVerbosity::Warning);
}

void FObsidianInventoryGrid::ChangedEntryStacks(UObsidianInventoryItemInstance* Instance, const int32 OldCount)
{
	const int32 NewCount = Instance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current);
	
	bool bSuccess = false;
	for(FObsidianInventoryEntry& Entry : Entries)
	{
		if(Entry.Instance == Instance)
		{
			Entry.StackCount = NewCount;
			MarkItemDirty(Entry);
			bSuccess = true;
		}
	}

	if(bSuccess)
	{
		const FVector2D GridLocation = Instance->GetItemCurrentGridLocation();
		BroadcastChangeMessage(Instance, OldCount, NewCount, GridLocation, EObsidianInventoryChangeType::ICT_ItemStacksChanged);
		return;
	}
	FFrame::KismetExecutionMessage(TEXT("Provided Instance to change is not in the Inventory List."), ELogVerbosity::Warning);
}

void FObsidianInventoryGrid::GeneralEntryChange(UObsidianInventoryItemInstance* Instance)
{
	bool bSuccess = false;
	for(FObsidianInventoryEntry& Entry : Entries)
	{
		if(Entry.Instance == Instance)
		{
			MarkItemDirty(Entry);
			bSuccess = true;
		}
	}
	
	if(bSuccess)
	{
		const FVector2D GridLocation = Instance->GetItemCurrentGridLocation();
		const int32 Count = Instance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current);
		BroadcastChangeMessage(Instance, Count, Count, GridLocation, EObsidianInventoryChangeType::ICT_GeneralItemChanged);
		return;
	}
	FFrame::KismetExecutionMessage(TEXT("Provided Instance to change is not in the Inventory List."), ELogVerbosity::Warning);
}

void FObsidianInventoryGrid::Item_MarkSpace(const UObsidianInventoryItemInstance* ItemInstance, const FVector2D AtPosition)
{
	const TArray<FVector2D> ItemGridSize = ItemInstance->GetItemGridSize();
	for(const FVector2D LocationComp : ItemGridSize)
	{
		const FVector2D Location = AtPosition + LocationComp;
		if(InventoryStateMap.Contains(Location))
		{
			InventoryStateMap[Location] = true;
		}
#if !UE_BUILD_SHIPPING
		else
		{
			FFrame::KismetExecutionMessage(*FString::Printf(TEXT("Trying to Mark a Location [x: %f, y: %f] that doesn't"
			 "exist in the InventoryStateMap in UObsidianInventoryComponent::Item_MarkSpace."), Location.X, Location.Y), ELogVerbosity::Error);
		}
#endif
	}
}

void FObsidianInventoryGrid::Item_UnMarkSpace(const UObsidianInventoryItemInstance* ItemInstance, const FVector2D AtPosition)
{
	const TArray<FVector2D> ItemGridSize = ItemInstance->GetItemGridSize();
	for(const FVector2D LocationComp : ItemGridSize)
	{
		const FVector2D Location = AtPosition + LocationComp;
		if(InventoryStateMap.Contains(Location))
		{
			InventoryStateMap[Location] = false;
		}
#if !UE_BUILD_SHIPPING
		else
		{
			FFrame::KismetExecutionMessage(*FString::Printf(TEXT("Trying to UnMark a Location [x: %f, y: %f] that doesn't"
			"exist in the InventoryStateMap in UObsidianInventoryComponent::Item_UnMarkSpace."), Location.X, Location.Y), ELogVerbosity::Error);
		}
#endif
	}
}

void FObsidianInventoryGrid::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	for(const int32 Index : RemovedIndices)
	{
		FObsidianInventoryEntry& Entry = Entries[Index];
		BroadcastChangeMessage(Entry, /* Old Count */ Entry.StackCount, /* New Count */ 0, Entry.GridLocation, EObsidianInventoryChangeType::ICT_ItemRemoved);
		Entry.LastObservedCount = 0;

		GridLocationToItemMap.Remove(Entry.GridLocation);
		Item_UnMarkSpace(Entry.Instance, Entry.GridLocation);
	}
}

void FObsidianInventoryGrid::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	UE_LOG(LogTemp, Warning, TEXT("PostReplicatedAdd, hi."));
	for(const int32 Index : AddedIndices)
	{
		FObsidianInventoryEntry& Entry = Entries[Index];
		BroadcastChangeMessage(Entry, /* Old Count */ 0, /* New Count */ Entry.StackCount, Entry.GridLocation, EObsidianInventoryChangeType::ICT_ItemAdded);
		Entry.LastObservedCount = Entry.StackCount;

		GridLocationToItemMap.Add(Entry.GridLocation, Entry.Instance);
		Item_MarkSpace(Entry.Instance, Entry.GridLocation);
	}
}

void FObsidianInventoryGrid::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	for(const int32 Index : ChangedIndices)
	{
		FObsidianInventoryEntry& Entry = Entries[Index];
		check(Entry.LastObservedCount != INDEX_NONE);
		if(Entry.LastObservedCount == Entry.StackCount)
		{
			BroadcastChangeMessage(Entry, /* Old Count */ Entry.LastObservedCount, /* New Count */ Entry.StackCount, Entry.GridLocation, EObsidianInventoryChangeType::ICT_GeneralItemChanged);
		}
		else
		{
			BroadcastChangeMessage(Entry, /* Old Count */ Entry.LastObservedCount, /* New Count */ Entry.StackCount, Entry.GridLocation, EObsidianInventoryChangeType::ICT_ItemStacksChanged);
		}
		Entry.LastObservedCount = Entry.StackCount;
	}
}

void FObsidianInventoryGrid::BroadcastChangeMessage(const FObsidianInventoryEntry& Entry, const int32 OldCount, const int32 NewCount, const FVector2D& GridPosition, const EObsidianInventoryChangeType& ChangeType) const
{
	FObsidianInventoryChangeMessage Message;
	Message.InventoryOwner = OwnerComponent;
	Message.ItemInstance = Entry.Instance;
	Message.NewCount = NewCount;
	Message.Delta = NewCount - OldCount;
	Message.GridItemPosition = GridPosition;
	Message.ChangeType = ChangeType;

	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(OwnerComponent->GetWorld());
	MessageSubsystem.BroadcastMessage(ObsidianGameplayTags::Message_Inventory_Changed, Message);
}


