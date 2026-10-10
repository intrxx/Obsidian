// Copyright 2026 out of sCope team - intrxx

#include "InventoryItems/Inventory/ObsidianInventoryGridItemList.h"

#include "GameFramework/GameplayMessageSubsystem.h"

#include "InventoryItems/Inventory/ObsidianInventoryComponent.h"
#include "InventoryItems/ObsidianInventoryItemDefinition.h"
#include "InventoryItems/ObsidianInventoryItemFragment.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "InventoryItems/ObsidianItemsFunctionLibrary.h"
#include "Obsidian/ObsidianGameplayTags.h"
#include "Obsidian/ObsidianLogCategories.h"


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

TArray<UObsidianInventoryItemInstance*> FObsidianInventoryGridItemList::GetAllItems() const
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

int32 FObsidianInventoryGridItemList::GetEntriesCount() const
{
	return Entries.Num();
}

UObsidianInventoryItemInstance* FObsidianInventoryGridItemList::AddEntry(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDefClass,
	const FObsidianItemGeneratedData& ItemGeneratedData, const int32 StackCount, const FIntPoint& AvailablePosition)
{
	check(ItemDefClass != nullptr);
	check(OwnerComponent);

	const AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor);

	FObsidianInventoryEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.Instance = UObsidianItemsFunctionLibrary::CreateItemInstanceFromDefinition(OwnerComponent->GetOwner(), ItemDefClass,
		ItemGeneratedData, AvailablePosition);
	NewEntry.StackCount = StackCount;
	NewEntry.GridLocation = AvailablePosition;
	
	UObsidianInventoryItemInstance* Item = NewEntry.Instance;
	
#if !UE_BUILD_SHIPPING
	if(GridLocationToItemMap.Contains(AvailablePosition))
	{
		FFrame::KismetExecutionMessage(*FString::Printf(TEXT("Provided Available Position [x: %d, y: %d] already"
			 "exist in the GridLocationToItemMap in [%hs]"), AvailablePosition.X, AvailablePosition.Y, __FUNCTION__), ELogVerbosity::Error);
	}
#endif

	GridLocationToItemMap.Add(AvailablePosition, Item);
	Item_MarkSpace(Item, AvailablePosition);
	
	MarkItemDirty(NewEntry);
	
	BroadcastChangeMessage(NewEntry, /* Old Count */ 0, /* New Count */ NewEntry.StackCount, AvailablePosition, EObsidianInventoryChangeType::ICT_ItemAdded);
	return Item;
}

void FObsidianInventoryGridItemList::AddEntry(UObsidianInventoryItemInstance* Instance, const FIntPoint& AvailablePosition)
{
	check(Instance != nullptr);
	check(OwnerComponent);

#if !UE_BUILD_SHIPPING
	if(GridLocationToItemMap.Contains(AvailablePosition))
	{
		FFrame::KismetExecutionMessage(*FString::Printf(TEXT("Provided Available Position [x: %d, y: %d] already"
			 "exist in the GridLocationToItemMap in [%hs]"), AvailablePosition.X, AvailablePosition.Y, __FUNCTION__), ELogVerbosity::Error);
	}
#endif

	FObsidianInventoryEntry& NewEntry = Entries.Emplace_GetRef(Instance);
	NewEntry.GridLocation = AvailablePosition; //TODO(intrxx) Add Grid Location to Entry instead of instance?
	NewEntry.Instance->SetItemCurrentPosition(AvailablePosition);
	NewEntry.StackCount = Instance->GetItemStackCount(ObsidianGameplayTags::Item::StackCount::Current);
	
	GridLocationToItemMap.Add(AvailablePosition, Instance);
	Item_MarkSpace(Instance, AvailablePosition);
	MarkItemDirty(NewEntry);
	
	BroadcastChangeMessage(NewEntry, /* Old Count */ 0, /* New Count */ NewEntry.StackCount, AvailablePosition, EObsidianInventoryChangeType::ICT_ItemAdded);
}

UObsidianInventoryItemInstance* FObsidianInventoryGridItemList::LoadEntry(const FObsidianSavedItem& EquippedSavedItem)
{
	check(OwnerComponent);
	
	UObsidianInventoryItemInstance* LoadedInstance = NewObject<UObsidianInventoryItemInstance>(OwnerComponent->GetOwner());
	LoadedInstance->ConstructFromSavedItem(EquippedSavedItem);

	const FIntPoint LoadedGridPosition = LoadedInstance->GetItemCurrentPosition().GetItemGridPosition();
	FObsidianInventoryEntry& NewEntry = Entries.Emplace_GetRef(LoadedInstance, LoadedGridPosition);
	NewEntry.StackCount = LoadedInstance->GetItemStackCount(ObsidianGameplayTags::Item::StackCount::Current);
	
	GridLocationToItemMap.Add(LoadedGridPosition, LoadedInstance);
	Item_MarkSpace(LoadedInstance, LoadedGridPosition);
	MarkItemDirty(NewEntry);
	
	BroadcastChangeMessage(NewEntry, /* Old Count */ 0, /* New Count */ NewEntry.StackCount, LoadedGridPosition, EObsidianInventoryChangeType::ICT_ItemAdded);

	return LoadedInstance;
}

void FObsidianInventoryGridItemList::RemoveEntry(UObsidianInventoryItemInstance* Instance)
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
		const FIntPoint CachedLocation = Instance->GetItemCurrentPosition().GetItemGridPosition();
		Instance->ResetItemCurrentPosition();
		
		GridLocationToItemMap.Remove(CachedLocation);
		Item_UnMarkSpace(Instance, CachedLocation);

		const int32 StackCount = Instance->GetItemStackCount(ObsidianGameplayTags::Item::StackCount::Current);
		BroadcastChangeMessage(Instance, /* Old Count */ StackCount, /* New Count */ 0, CachedLocation, EObsidianInventoryChangeType::ICT_ItemRemoved);
		return;
	}
	FFrame::KismetExecutionMessage(TEXT("Provided Instance to remove is not in the Inventory List."), ELogVerbosity::Warning);
}

void FObsidianInventoryGridItemList::ChangedEntryStacks(UObsidianInventoryItemInstance* Instance, const int32 OldCount)
{
	const int32 NewCount = Instance->GetItemStackCount(ObsidianGameplayTags::Item::StackCount::Current);
	
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
		const FIntPoint GridLocation = Instance->GetItemCurrentPosition().GetItemGridPosition();
		BroadcastChangeMessage(Instance, OldCount, NewCount, GridLocation, EObsidianInventoryChangeType::ICT_ItemStacksChanged);
		return;
	}
	FFrame::KismetExecutionMessage(TEXT("Provided Instance to change is not in the Inventory List."), ELogVerbosity::Warning);
}

void FObsidianInventoryGridItemList::GeneralEntryChange(UObsidianInventoryItemInstance* Instance)
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
		const FIntPoint GridLocation = Instance->GetItemCurrentPosition().GetItemGridPosition();
		const int32 Count = Instance->GetItemStackCount(ObsidianGameplayTags::Item::StackCount::Current);
		BroadcastChangeMessage(Instance, Count, Count, GridLocation,
			EObsidianInventoryChangeType::ICT_GeneralItemChanged);
		return;
	}
	
	FFrame::KismetExecutionMessage(TEXT("Provided Instance to change is not in the Inventory List."),
		ELogVerbosity::Warning);
}

void FObsidianInventoryGridItemList::Item_MarkSpace(const UObsidianInventoryItemInstance* ItemInstance, const FIntPoint& AtPosition)
{
	const FIntPoint ItemGridSpan = ItemInstance->GetItemGridSpan();
	for(int32 SpanX = 0; SpanX < ItemGridSpan.X; ++SpanX)
	{
		for(int32 SpanY = 0; SpanY < ItemGridSpan.Y; ++SpanY)
		{
			const FIntPoint LocationToMark = AtPosition + FIntPoint(SpanX, SpanY);
			if(bool* Location = InventoryStateMap.Find(LocationToMark))
			{
				*Location = true;
			}
#if !UE_BUILD_SHIPPING
			else
			{
				FFrame::KismetExecutionMessage(*FString::Printf(TEXT("Trying to Mark a Location [x: %d, y: %d] that doesn't"
				 "exist in the InventoryStateMap in UObsidianInventoryComponent::Item_MarkSpace."), LocationToMark.X, LocationToMark.Y), ELogVerbosity::Error);
			}
#endif
		}
	}
}

void FObsidianInventoryGridItemList::Item_UnMarkSpace(const UObsidianInventoryItemInstance* ItemInstance, const FIntPoint& AtPosition)
{
	const FIntPoint ItemGridSpan = ItemInstance->GetItemGridSpan();
	for(int32 SpanX = 0; SpanX < ItemGridSpan.X; ++SpanX)
	{
		for(int32 SpanY = 0; SpanY < ItemGridSpan.Y; ++SpanY)
		{
			const FIntPoint LocationToUnmark = AtPosition + FIntPoint(SpanX, SpanY);
			if(bool* Location = InventoryStateMap.Find(LocationToUnmark))
			{
				*Location = false;
			}
#if !UE_BUILD_SHIPPING
			else
			{
				FFrame::KismetExecutionMessage(*FString::Printf(TEXT("Trying to UnMark a Location [x: %d, y: %d] that doesn't"
				"exist in the InventoryStateMap in UObsidianInventoryComponent::Item_UnMarkSpace."), LocationToUnmark.X, LocationToUnmark.Y), ELogVerbosity::Error);
			}
#endif
		}
	}
}

void FObsidianInventoryGridItemList::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	for(const int32 Index : RemovedIndices)
	{
		FObsidianInventoryEntry& Entry = Entries[Index];
		if(Entry.Instance == nullptr || Entry.LastObservedCount == INDEX_NONE) // Item was never added on this Client.
		{
			continue;
		}

		BroadcastChangeMessage(Entry, /* Old Count */ Entry.StackCount, /* New Count */ 0, Entry.GridLocation, EObsidianInventoryChangeType::ICT_ItemRemoved);
		Entry.LastObservedCount = 0;

		GridLocationToItemMap.Remove(Entry.GridLocation);
		Item_UnMarkSpace(Entry.Instance, Entry.GridLocation);

		UE_LOG(ObLogInventory, Verbose, TEXT("Replicated removing [%s] item."), *Entry.Instance->GetItemDebugName());
	}
}

void FObsidianInventoryGridItemList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for(const int32 Index : AddedIndices)
	{
		FObsidianInventoryEntry& Entry = Entries[Index];
		if(Entry.Instance == nullptr)
		{
			// The Item Instance subobject did not arrive yet, PostReplicatedChange will add the Item once it gets resolved.
			UE_LOG(ObLogInventory, Verbose, TEXT("Replicated Item at index [%d] has no Instance yet, deferring."), Index);
			continue;
		}

		BroadcastChangeMessage(Entry, /* Old Count */ 0, /* New Count */ Entry.StackCount, Entry.GridLocation, EObsidianInventoryChangeType::ICT_ItemAdded);
		Entry.LastObservedCount = Entry.StackCount;

		GridLocationToItemMap.Add(Entry.GridLocation, Entry.Instance);
		Item_MarkSpace(Entry.Instance, Entry.GridLocation);

		UE_LOG(ObLogInventory, Verbose, TEXT("Replicated adding [%s] item."), *Entry.Instance->GetItemDebugName());
	}
}

void FObsidianInventoryGridItemList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	for(const int32 Index : ChangedIndices)
	{
		FObsidianInventoryEntry& Entry = Entries[Index];
		if(Entry.Instance == nullptr)
		{
			continue;
		}

		if(Entry.LastObservedCount == INDEX_NONE) // Adding was deferred until the Item Instance got resolved.
		{
			int32 AddedIndex = Index;
			PostReplicatedAdd(MakeArrayView(&AddedIndex, 1), FinalSize);
			continue;
		}

		if(Entry.LastObservedCount == Entry.StackCount)
		{
			BroadcastChangeMessage(Entry, /* Old Count */ Entry.LastObservedCount, /* New Count */ Entry.StackCount, Entry.GridLocation, EObsidianInventoryChangeType::ICT_GeneralItemChanged);
		}
		else
		{
			BroadcastChangeMessage(Entry, /* Old Count */ Entry.LastObservedCount, /* New Count */ Entry.StackCount, Entry.GridLocation, EObsidianInventoryChangeType::ICT_ItemStacksChanged);
		}
		Entry.LastObservedCount = Entry.StackCount;

		UE_LOG(ObLogInventory, Verbose, TEXT("Replicated changing [%s] item."), *Entry.Instance->GetItemDebugName());
	}
}

void FObsidianInventoryGridItemList::BroadcastChangeMessage(const FObsidianInventoryEntry& Entry, const int32 OldCount, const int32 NewCount, const FIntPoint& GridPosition, const EObsidianInventoryChangeType& ChangeType) const
{
	FObsidianInventoryChangeMessage Message;
	Message.InventoryOwner = OwnerComponent;
	Message.ItemInstance = Entry.Instance;
	Message.NewCount = NewCount;
	Message.Delta = NewCount - OldCount;
	Message.GridItemPosition = GridPosition;
	Message.ChangeType = ChangeType;

	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(OwnerComponent->GetWorld());
	MessageSubsystem.BroadcastMessage(ObsidianGameplayTags::Message::Inventory::Changed, Message);
}


