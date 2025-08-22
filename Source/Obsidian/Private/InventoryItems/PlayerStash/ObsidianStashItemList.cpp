// Copyright 2024 out of sCope team - Michał Ogiński

#include "InventoryItems/PlayerStash/ObsidianStashItemList.h"

// ~ Core
#include "GameFramework/GameplayMessageSubsystem.h"

// ~ Project
#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "InventoryItems/PlayerStash/ObsidianPlayerStashComponent.h"
#include "InventoryItems/PlayerStash/ObsidianStashTab.h"
#include "InventoryItems/PlayerStash/ObsidianStashTabsConfig.h"

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

void FObsidianStashItemList::InitializeStashTabs(const UObsidianStashTabsConfig* StashTabsConfig)
{
	if(StashTabsConfig == nullptr || OwnerComponent == nullptr)
	{
		return;
	}
	
	UObsidianPlayerStashComponent* StashTabComponent = Cast<UObsidianPlayerStashComponent>(OwnerComponent);
	if(StashTabComponent == nullptr)
	{
		return;
	}

	const TArray<FObsidianStashTabDefinition> StashTabDefinitions = StashTabsConfig->GetStashTabDefinitions();
	StashTabsMap.Empty(StashTabDefinitions.Num());
	
	for(const FObsidianStashTabDefinition& Definition : StashTabDefinitions)
	{
		if(Definition.StashTabClass == nullptr)
		{
			continue;
		}

		UObsidianStashTab* NewTab = NewObject<UObsidianStashTab>(OwnerComponent, Definition.StashTabClass);
		check(NewTab);
		
		NewTab->SetStashTabTag(Definition.StashTag);
		NewTab->Construct(StashTabComponent);

		StashTabsMap.Add(Definition.StashTag,NewTab);
	}
}

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

TArray<UObsidianInventoryItemInstance*> FObsidianStashItemList::GetAllItemsFromStashTab(const FGameplayTag& ForStashTabTag)
{
	TArray<UObsidianInventoryItemInstance*> Items;

	for(const FObsidianStashEntry& Entry : Entries)
	{
		if(Entry.Instance && Entry.StashTabTag == ForStashTabTag)
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

UObsidianStashTab* FObsidianStashItemList::GetStashTabForTag(const FGameplayTag& StashTabTag)
{
	if(UObsidianStashTab** StashTabPointer = StashTabsMap.Find(StashTabTag))
	{
		return *StashTabPointer;
	}
	return nullptr;
}

UObsidianInventoryItemInstance* FObsidianStashItemList::AddEntry(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDefClass, const int32 StackCount, const FGameplayTag& StashTabTag, const FObsidianItemPosition& ToPosition)
{
	check(ItemDefClass != nullptr);
	check(OwnerComponent);

	const AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor);

	UObsidianStashTab* StashTab = GetStashTabForTag(StashTabTag);
	if(StashTab == nullptr)
	{
		UE_LOG(LogPlayerStash, Error, TEXT("StashTab for provided tag is invalid in [%hs]"), ANSI_TO_TCHAR(__FUNCTION__));
		return nullptr;
	}

#if !UE_BUILD_SHIPPING
	if(StashTab->VerifyPositionFree(ToPosition) == false)
	{
		FFrame::KismetExecutionMessage(*FString::Printf(TEXT("Provided Available Position [x: %d, y: %d] already"
			 "exist in the StashTab's Map in [%ls]"), ToPosition.GetItemGridLocation().X, ToPosition.GetItemGridLocation().Y, ANSI_TO_TCHAR(__FUNCTION__)),
			 ELogVerbosity::Error);
	}
#endif

	FObsidianStashEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.Instance = NewObject<UObsidianInventoryItemInstance>(OwnerComponent->GetOwner());
	NewEntry.Instance->SetItemDef(ItemDefClass);
	NewEntry.Instance->SetItemCurrentPosition(ToPosition);

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
	NewEntry.ItemPosition = ToPosition;
	NewEntry.StashTabTag = StashTabTag;
	NewEntry.Instance->OnInstanceCreatedAndInitialized();
	
	UObsidianInventoryItemInstance* Item = NewEntry.Instance;
	
	StashTab->MarkSpaceInTab(Item, ToPosition);
	MarkItemDirty(NewEntry);
	
	BroadcastChangeMessage(NewEntry, /* Old Count */ 0, /* New Count */ NewEntry.StackCount, StashTabTag, ToPosition, EObsidianStashChangeType::ICT_ItemAdded);
	return Item;
}

void FObsidianStashItemList::AddEntry(UObsidianInventoryItemInstance* Instance, const FGameplayTag& StashTabTag, const FObsidianItemPosition& ToPosition)
{
	check(Instance != nullptr);
	check(OwnerComponent);

	UObsidianStashTab* StashTab = GetStashTabForTag(StashTabTag);
	if(StashTab == nullptr)
	{
		UE_LOG(LogPlayerStash, Error, TEXT("StashTab for provided tag is invalid in [%hs]"), ANSI_TO_TCHAR(__FUNCTION__));
		return;
	}

#if !UE_BUILD_SHIPPING
	if(StashTab->VerifyPositionFree(ToPosition) == false)
	{
		FFrame::KismetExecutionMessage(*FString::Printf(TEXT("Provided Available Position [x: %d, y: %d] already"
			 "exist in the StashTab's Map in [%ls]"), ToPosition.GetItemGridLocation().X, ToPosition.GetItemGridLocation().Y, ANSI_TO_TCHAR(__FUNCTION__)),
			 ELogVerbosity::Error);
	}
#endif

	FObsidianStashEntry& NewEntry = Entries.Emplace_GetRef(Instance);
	NewEntry.ItemPosition = ToPosition;
	NewEntry.Instance->SetItemCurrentPosition(ToPosition);
	NewEntry.StackCount = Instance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current);
	NewEntry.StashTabTag = StashTabTag;
	
	StashTab->MarkSpaceInTab(Instance, ToPosition);
	MarkItemDirty(NewEntry);
	
	BroadcastChangeMessage(NewEntry, /* Old Count */ 0, /* New Count */ NewEntry.StackCount, StashTabTag, ToPosition, EObsidianStashChangeType::ICT_ItemAdded);
}

void FObsidianStashItemList::RemoveEntry(UObsidianInventoryItemInstance* Instance, const FGameplayTag& StashTabTag)
{
	UObsidianStashTab* StashTab = GetStashTabForTag(StashTabTag);
	if(StashTab == nullptr)
	{
		UE_LOG(LogPlayerStash, Error, TEXT("StashTab for provided tag is invalid in [%hs]"), ANSI_TO_TCHAR(__FUNCTION__));
		return;
	}

	FGameplayTag ItemStashTabTag = FGameplayTag::EmptyTag;
	bool bSuccess = false;
	
	for(auto It = Entries.CreateIterator(); It; ++It)
	{
		FObsidianStashEntry& Entry = *It;
		if(Entry.Instance == Instance)
		{
			ItemStashTabTag = Entry.StashTabTag;
			It.RemoveCurrent();
			MarkArrayDirty();
			bSuccess = true;
		}
	}

	if(bSuccess)
	{
		const FObsidianItemPosition CachedPosition = Instance->GetItemCurrentPosition();
		Instance->ResetItemCurrentPosition();

		ensure(StashTabTag == ItemStashTabTag);
		StashTab->UnmarkSpaceInTab(Instance, CachedPosition);

		const int32 StackCount = Instance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current);
		BroadcastChangeMessage(Instance, /* Old Count */ StackCount, /* New Count */ 0, ItemStashTabTag, CachedPosition, EObsidianStashChangeType::ICT_ItemRemoved);
		return;
	}
	FFrame::KismetExecutionMessage(TEXT("Provided Instance to remove is not in the Inventory List."), ELogVerbosity::Warning);
}

void FObsidianStashItemList::ChangedEntryStacks(UObsidianInventoryItemInstance* Instance, const int32 OldCount, const FGameplayTag& StashTabTag)
{
	const int32 NewCount = Instance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current);
	
	bool bSuccess = false;
	for(FObsidianStashEntry& Entry : Entries)
	{
		if(Entry.Instance == Instance)
		{
			ensure(StashTabTag == Entry.StashTabTag);
			Entry.StackCount = NewCount;
			MarkItemDirty(Entry);
			bSuccess = true;
		}
	}

	if(bSuccess)
	{
		BroadcastChangeMessage(Instance, OldCount, NewCount, StashTabTag, Instance->GetItemCurrentPosition(), EObsidianStashChangeType::ICT_ItemStacksChanged);
		return;
	}
	FFrame::KismetExecutionMessage(TEXT("Provided Instance to change is not in the Inventory List."), ELogVerbosity::Warning);
}

void FObsidianStashItemList::GeneralEntryChange(UObsidianInventoryItemInstance* Instance, const FGameplayTag& StashTabTag)
{
	bool bSuccess = false;
	for(FObsidianStashEntry& Entry : Entries)
	{
		if(Entry.Instance == Instance)
		{
			MarkItemDirty(Entry);
			bSuccess = true;
		}
	}
	
	if(bSuccess)
	{
		const int32 Count = Instance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current);
		BroadcastChangeMessage(Instance, Count, Count, StashTabTag, Instance->GetItemCurrentPosition(), EObsidianStashChangeType::ICT_GeneralItemChanged);
		return;
	}
	FFrame::KismetExecutionMessage(TEXT("Provided Instance to change is not in the Inventory List."), ELogVerbosity::Warning);
}

void FObsidianStashItemList::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	for(const int32 Index : RemovedIndices)
	{
		FObsidianStashEntry& Entry = Entries[Index];
		if (UObsidianStashTab* StashTab = GetStashTabForTag(Entry.StashTabTag))
		{
			BroadcastChangeMessage(Entry, /* Old Count */ Entry.StackCount, /* New Count */ 0, Entry.StashTabTag, Entry.ItemPosition, EObsidianStashChangeType::ICT_ItemRemoved);
			Entry.LastObservedCount = 0;
			
			StashTab->UnmarkSpaceInTab(Entry.Instance, Entry.ItemPosition);

			UE_LOG(LogPlayerStash, Display, TEXT("Replicated removing [%s] item."), *Entry.Instance->GetItemDebugName());
		}
	}
}

void FObsidianStashItemList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for(const int32 Index : AddedIndices)
	{
		FObsidianStashEntry& Entry = Entries[Index];
		if (UObsidianStashTab* StashTab = GetStashTabForTag(Entry.StashTabTag))
		{
			BroadcastChangeMessage(Entry, /* Old Count */ 0, /* New Count */ Entry.StackCount, Entry.StashTabTag, Entry.ItemPosition, EObsidianStashChangeType::ICT_ItemAdded);
			Entry.LastObservedCount = Entry.StackCount;
			
			StashTab->MarkSpaceInTab(Entry.Instance, Entry.ItemPosition);

			UE_LOG(LogPlayerStash, Display, TEXT("Replicated adding [%s] item."), *Entry.Instance->GetItemDebugName());
		}
	}
}

void FObsidianStashItemList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	for(const int32 Index : ChangedIndices)
	{
		FObsidianStashEntry& Entry = Entries[Index];
		check(Entry.LastObservedCount != INDEX_NONE);
		if(Entry.LastObservedCount == Entry.StackCount)
		{
			BroadcastChangeMessage(Entry, /* Old Count */ Entry.LastObservedCount, /* New Count */ Entry.StackCount, Entry.StashTabTag, Entry.ItemPosition, EObsidianStashChangeType::ICT_GeneralItemChanged);
		}
		else
		{
			BroadcastChangeMessage(Entry, /* Old Count */ Entry.LastObservedCount, /* New Count */ Entry.StackCount, Entry.StashTabTag, Entry.ItemPosition, EObsidianStashChangeType::ICT_ItemStacksChanged);
		}
		Entry.LastObservedCount = Entry.StackCount;

		UE_LOG(LogPlayerStash, Display, TEXT("Replicated changing [%s] item."), *Entry.Instance->GetItemDebugName());
	}
}

void FObsidianStashItemList::BroadcastChangeMessage(const FObsidianStashEntry& Entry, const int32 OldCount, const int32 NewCount, const FGameplayTag& StashTabTag, const FObsidianItemPosition& ItemPosition, const EObsidianStashChangeType& ChangeType) const
{
	FObsidianStashChangeMessage Message;
	Message.InventoryOwner = OwnerComponent;
	Message.ItemInstance = Entry.Instance;
	Message.StashTabTag = StashTabTag;
	Message.ItemPosition = ItemPosition;
	Message.NewCount = NewCount;
	Message.Delta = NewCount - OldCount;
	Message.ChangeType = ChangeType;

	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(OwnerComponent->GetWorld());
	MessageSubsystem.BroadcastMessage(ObsidianGameplayTags::Message_PlayerStash_Changed, Message);
}

