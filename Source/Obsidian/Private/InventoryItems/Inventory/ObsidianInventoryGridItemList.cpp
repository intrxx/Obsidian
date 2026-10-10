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

UObsidianInventoryItemInstance* FObsidianInventoryGridItemList::AddEntry(const TSubclassOf<UObsidianInventoryItemDefinition>& InItemDefClass,
	const FObsidianItemGeneratedData& InItemGeneratedData, const int32 InStackCount, const FIntPoint& InAvailablePosition)
{
	check(InItemDefClass != nullptr);
	check(OwnerComponent);

	const AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor);

	FObsidianInventoryEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.Instance = UObsidianItemsFunctionLibrary::CreateItemInstanceFromDefinition(OwnerComponent->GetOwner(), InItemDefClass,
		InItemGeneratedData, InAvailablePosition);
	NewEntry.StackCount = InStackCount;
	NewEntry.GridLocation = InAvailablePosition;
	
	UObsidianInventoryItemInstance* Item = NewEntry.Instance;
	
#if !UE_BUILD_SHIPPING
	if(GridLocationToItemMap.Contains(InAvailablePosition))
	{
		FFrame::KismetExecutionMessage(*FString::Printf(TEXT("Provided Available Position [x: %d, y: %d] already"
			 "exist in the GridLocationToItemMap in [%hs]"), InAvailablePosition.X, InAvailablePosition.Y, __FUNCTION__), ELogVerbosity::Error);
	}
#endif

	GridLocationToItemMap.Add(InAvailablePosition, Item);
	Item_MarkSpace(Item, InAvailablePosition);
	
	MarkItemDirty(NewEntry);
	
	BroadcastChangeMessage(NewEntry, /* Old Count */ 0, /* New Count */ NewEntry.StackCount, InAvailablePosition, EObsidianInventoryChangeType::ICT_ItemAdded);
	return Item;
}

void FObsidianInventoryGridItemList::AddEntry(UObsidianInventoryItemInstance* InInstance, const FIntPoint& InAvailablePosition)
{
	check(InInstance != nullptr);
	check(OwnerComponent);

#if !UE_BUILD_SHIPPING
	if(GridLocationToItemMap.Contains(InAvailablePosition))
	{
		FFrame::KismetExecutionMessage(*FString::Printf(TEXT("Provided Available Position [x: %d, y: %d] already"
			 "exist in the GridLocationToItemMap in [%hs]"), InAvailablePosition.X, InAvailablePosition.Y, __FUNCTION__), ELogVerbosity::Error);
	}
#endif

	FObsidianInventoryEntry& NewEntry = Entries.Emplace_GetRef(InInstance);
	NewEntry.GridLocation = InAvailablePosition; //TODO(intrxx) Add Grid Location to Entry instead of instance?
	NewEntry.Instance->SetItemCurrentPosition(InAvailablePosition);
	NewEntry.StackCount = InInstance->GetItemStackCount(ObsidianGameplayTags::Item::StackCount::Current);
	
	GridLocationToItemMap.Add(InAvailablePosition, InInstance);
	Item_MarkSpace(InInstance, InAvailablePosition);
	MarkItemDirty(NewEntry);
	
	BroadcastChangeMessage(NewEntry, /* Old Count */ 0, /* New Count */ NewEntry.StackCount, InAvailablePosition, EObsidianInventoryChangeType::ICT_ItemAdded);
}

UObsidianInventoryItemInstance* FObsidianInventoryGridItemList::LoadEntry(const FObsidianSavedItem& InEquippedSavedItem)
{
	check(OwnerComponent);
	
	UObsidianInventoryItemInstance* LoadedInstance = NewObject<UObsidianInventoryItemInstance>(OwnerComponent->GetOwner());
	LoadedInstance->ConstructFromSavedItem(InEquippedSavedItem);

	const FIntPoint LoadedGridPosition = LoadedInstance->GetItemCurrentPosition().GetItemGridPosition();
	FObsidianInventoryEntry& NewEntry = Entries.Emplace_GetRef(LoadedInstance, LoadedGridPosition);
	NewEntry.StackCount = LoadedInstance->GetItemStackCount(ObsidianGameplayTags::Item::StackCount::Current);
	
	GridLocationToItemMap.Add(LoadedGridPosition, LoadedInstance);
	Item_MarkSpace(LoadedInstance, LoadedGridPosition);
	MarkItemDirty(NewEntry);
	
	BroadcastChangeMessage(NewEntry, /* Old Count */ 0, /* New Count */ NewEntry.StackCount, LoadedGridPosition, EObsidianInventoryChangeType::ICT_ItemAdded);

	return LoadedInstance;
}

void FObsidianInventoryGridItemList::RemoveEntry(UObsidianInventoryItemInstance* InInstance)
{
	bool bSuccess = false;
	for(auto It = Entries.CreateIterator(); It; ++It)
	{
		FObsidianInventoryEntry& Entry = *It;
		if(Entry.Instance == InInstance)
		{
			It.RemoveCurrent();
			MarkArrayDirty();
			bSuccess = true;
		}
	}

	if(bSuccess)
	{
		const FIntPoint CachedLocation = InInstance->GetItemCurrentPosition().GetItemGridPosition();
		InInstance->ResetItemCurrentPosition();
		
		GridLocationToItemMap.Remove(CachedLocation);
		Item_UnMarkSpace(InInstance, CachedLocation);

		const int32 StackCount = InInstance->GetItemStackCount(ObsidianGameplayTags::Item::StackCount::Current);
		BroadcastChangeMessage(InInstance, /* Old Count */ StackCount, /* New Count */ 0, CachedLocation, EObsidianInventoryChangeType::ICT_ItemRemoved);
		return;
	}
	FFrame::KismetExecutionMessage(TEXT("Provided Instance to remove is not in the Inventory List."), ELogVerbosity::Warning);
}

void FObsidianInventoryGridItemList::ChangedEntryStacks(UObsidianInventoryItemInstance* InInstance, const int32 InOldCount)
{
	const int32 NewCount = InInstance->GetItemStackCount(ObsidianGameplayTags::Item::StackCount::Current);
	
	bool bSuccess = false;
	for(FObsidianInventoryEntry& Entry : Entries)
	{
		if(Entry.Instance == InInstance)
		{
			Entry.StackCount = NewCount;
			MarkItemDirty(Entry);
			bSuccess = true;
		}
	}

	if(bSuccess)
	{
		const FIntPoint GridLocation = InInstance->GetItemCurrentPosition().GetItemGridPosition();
		BroadcastChangeMessage(InInstance, InOldCount, NewCount, GridLocation, EObsidianInventoryChangeType::ICT_ItemStacksChanged);
		return;
	}
	FFrame::KismetExecutionMessage(TEXT("Provided Instance to change is not in the Inventory List."), ELogVerbosity::Warning);
}

void FObsidianInventoryGridItemList::GeneralEntryChange(UObsidianInventoryItemInstance* InInstance)
{
	bool bSuccess = false;
	for(FObsidianInventoryEntry& Entry : Entries)
	{
		if(Entry.Instance == InInstance)
		{
			MarkItemDirty(Entry);
			bSuccess = true;
		}
	}
	
	if(bSuccess)
	{
		const FIntPoint GridLocation = InInstance->GetItemCurrentPosition().GetItemGridPosition();
		const int32 Count = InInstance->GetItemStackCount(ObsidianGameplayTags::Item::StackCount::Current);
		BroadcastChangeMessage(InInstance, Count, Count, GridLocation,
			EObsidianInventoryChangeType::ICT_GeneralItemChanged);
		return;
	}
	
	FFrame::KismetExecutionMessage(TEXT("Provided Instance to change is not in the Inventory List."),
		ELogVerbosity::Warning);
}

void FObsidianInventoryGridItemList::Item_MarkSpace(const UObsidianInventoryItemInstance* InItemInstance, const FIntPoint& InAtPosition)
{
	const FIntPoint ItemGridSpan = InItemInstance->GetItemGridSpan();
	for(int32 SpanX = 0; SpanX < ItemGridSpan.X; ++SpanX)
	{
		for(int32 SpanY = 0; SpanY < ItemGridSpan.Y; ++SpanY)
		{
			const FIntPoint LocationToMark = InAtPosition + FIntPoint(SpanX, SpanY);
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

void FObsidianInventoryGridItemList::Item_UnMarkSpace(const UObsidianInventoryItemInstance* InItemInstance, const FIntPoint& InAtPosition)
{
	const FIntPoint ItemGridSpan = InItemInstance->GetItemGridSpan();
	for(int32 SpanX = 0; SpanX < ItemGridSpan.X; ++SpanX)
	{
		for(int32 SpanY = 0; SpanY < ItemGridSpan.Y; ++SpanY)
		{
			const FIntPoint LocationToUnmark = InAtPosition + FIntPoint(SpanX, SpanY);
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

void FObsidianInventoryGridItemList::PreReplicatedRemove(const TArrayView<int32> InRemovedIndices, int32 InFinalSize)
{
	for(const int32 Index : InRemovedIndices)
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

void FObsidianInventoryGridItemList::PostReplicatedAdd(const TArrayView<int32> InAddedIndices, int32 InFinalSize)
{
	for(const int32 Index : InAddedIndices)
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

void FObsidianInventoryGridItemList::PostReplicatedChange(const TArrayView<int32> InChangedIndices, int32 InFinalSize)
{
	for(const int32 Index : InChangedIndices)
	{
		FObsidianInventoryEntry& Entry = Entries[Index];
		if(Entry.Instance == nullptr)
		{
			continue;
		}

		if(Entry.LastObservedCount == INDEX_NONE) // Adding was deferred until the Item Instance got resolved.
		{
			int32 AddedIndex = Index;
			PostReplicatedAdd(MakeArrayView(&AddedIndex, 1), InFinalSize);
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

void FObsidianInventoryGridItemList::BroadcastChangeMessage(const FObsidianInventoryEntry& InEntry, const int32 InOldCount, const int32 InNewCount, const FIntPoint& InGridPosition, const EObsidianInventoryChangeType& InChangeType) const
{
	FObsidianInventoryChangeMessage Message;
	Message.InventoryOwner = OwnerComponent;
	Message.ItemInstance = InEntry.Instance;
	Message.NewCount = InNewCount;
	Message.Delta = InNewCount - InOldCount;
	Message.GridItemPosition = InGridPosition;
	Message.ChangeType = InChangeType;

	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(OwnerComponent->GetWorld());
	MessageSubsystem.BroadcastMessage(ObsidianGameplayTags::Message::Inventory::Changed, Message);
}


