// Copyright 2026 out of sCope team - intrxx

#include "InventoryItems/PlayerStash/ObsidianStashItemList.h"

#include "GameFramework/GameplayMessageSubsystem.h"

#include "InventoryItems/ObsidianInventoryItemDefinition.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "InventoryItems/ObsidianItemsFunctionLibrary.h"
#include "InventoryItems/PlayerStash/ObsidianPlayerStashComponent.h"
#include "InventoryItems/PlayerStash/ObsidianStashTab.h"
#include "InventoryItems/PlayerStash/ObsidianStashTabsConfig.h"
#include "InventoryItems/PlayerStash/Tabs/ObsidianStashTab_Slots.h"
#include "Obsidian/ObsidianLogCategories.h"


// ~ FObsidianStashSlotDefinition

FObsidianStashSlotDefinition const FObsidianStashSlotDefinition::InvalidSlot;

bool FObsidianStashSlotDefinition::IsValid() const
{
	return BaseSlotDefinition.IsValid();
}

bool FObsidianStashSlotDefinition::HasLimitedStacks() const
{
	return SlotStackLimit != INDEX_NONE;
}

FGameplayTag FObsidianStashSlotDefinition::GetStashSlotTag() const
{
	return BaseSlotDefinition.GetSlotTag();
}

EObsidianPlacingAtSlotResult FObsidianStashSlotDefinition::CanStashAtSlot(const FGameplayTag& InItemCategory, const FGameplayTag& InItemBaseType) const
{
	if (bRequireUniqueBaseTypeMatch)
	{
		return UniqueBaseTypeTag == InItemBaseType ? EObsidianPlacingAtSlotResult::CanPlace : EObsidianPlacingAtSlotResult::UnableToPlace_BaseTypeDiffers;
	}
	
	return BaseSlotDefinition.CanPlaceAtSlot(InItemCategory);
}

void FObsidianStashSlotDefinition::AddBannedStashCategory(const FGameplayTag& InBannedCategory)
{
	BaseSlotDefinition.AddBannedItemCategory(InBannedCategory);
}

void FObsidianStashSlotDefinition::AddBannedStashCategories(const FGameplayTagContainer& InBannedCategories)
{
	BaseSlotDefinition.AddBannedItemCategories(InBannedCategories);
}

void FObsidianStashSlotDefinition::RemoveBannedStashCategory(const FGameplayTag& InBannedCategoryToRemove)
{
	BaseSlotDefinition.RemoveBannedItemCategory(InBannedCategoryToRemove);
}

void FObsidianStashSlotDefinition::RemoveBannedStashCategories(const FGameplayTagContainer& InBannedCategoriesToRemove)
{
	BaseSlotDefinition.RemoveBannedItemCategories(InBannedCategoriesToRemove);
}

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

TArray<UObsidianStashTab*> FObsidianStashItemList::InitializeStashTabs(const UObsidianStashTabsConfig* InStashTabsConfig)
{
	UE_LOG(ObLogPlayerStash, Verbose, TEXT("Initializing Stash Tabs"));
	
	TArray<UObsidianStashTab*> InitializedStashTabs;
	
	if(InStashTabsConfig == nullptr || OwnerComponent == nullptr)
	{
		return InitializedStashTabs;
	}
	
	UObsidianPlayerStashComponent* StashTabComponent = Cast<UObsidianPlayerStashComponent>(OwnerComponent);
	if(StashTabComponent == nullptr)
	{
		return InitializedStashTabs;
	}

	const TArray<FObsidianStashTabDefinition> StashTabDefinitions = InStashTabsConfig->GetStashTabDefinitions();
	StashTabsMap.Empty(StashTabDefinitions.Num());
	
	for(const FObsidianStashTabDefinition& Definition : StashTabDefinitions)
	{
		if(Definition.StashTabClass == nullptr)
		{
			continue;
		}

		UObsidianStashTab* NewTab = NewObject<UObsidianStashTab>(OwnerComponent, Definition.StashTabClass);
		check(NewTab);
		NewTab->SetStashData(Definition);
		NewTab->Construct(StashTabComponent);
		
		StashTabsMap.Add(Definition.StashTag, NewTab);
		InitializedStashTabs.Add(NewTab);
	}
	
	return InitializedStashTabs;
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

TArray<UObsidianInventoryItemInstance*> FObsidianStashItemList::GetAllPersonalItems() const
{
	TArray<UObsidianInventoryItemInstance*> Items;
	
	for(const FObsidianStashEntry& Entry : Entries)
	{
		if(Entry.Instance && Entry.OwningStashTab && Entry.OwningStashTab->GetStashAccessabilityType() == EObsidianStashTabAccessability::Personal)
		{
			Items.Add(Entry.Instance);
		}
	}
	
	return Items;
}

TArray<UObsidianInventoryItemInstance*> FObsidianStashItemList::GetAllSharedItems() const
{
	TArray<UObsidianInventoryItemInstance*> Items;
	Items.Reserve(Entries.Num());
	
	for(const FObsidianStashEntry& Entry : Entries)
	{
		if(Entry.Instance && Entry.OwningStashTab && Entry.OwningStashTab->GetStashAccessabilityType() == EObsidianStashTabAccessability::Shared)
		{
			Items.Add(Entry.Instance);
		}
	}
	
	return Items;
}

TArray<UObsidianInventoryItemInstance*> FObsidianStashItemList::GetAllItemsFromStashTab(const FGameplayTag& InStashTabTag)
{
	TArray<UObsidianInventoryItemInstance*> Items;

	for(const FObsidianStashEntry& Entry : Entries)
	{
		if(Entry.Instance && Entry.ItemPosition.GetOwningStashTabTag() == InStashTabTag)
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

UObsidianStashTab* FObsidianStashItemList::GetStashTabForTag(const FGameplayTag& InStashTabTag)
{
	if(UObsidianStashTab** StashTabPointer = StashTabsMap.Find(InStashTabTag))
	{
		return *StashTabPointer;
	}
	return nullptr;
}

TArray<FObsidianStashSlotDefinition> FObsidianStashItemList::FindMatchingSlotsForItemCategory(const FGameplayTag& InItemCategory, const FGameplayTag& InItemBaseType, const UObsidianStashTab_Slots* InSlotStashTab)
{
	TArray<FObsidianStashSlotDefinition> MatchingSlots;
	if (InSlotStashTab == nullptr)
	{
		return MatchingSlots;
	}
	
	for (const FObsidianStashSlotDefinition& Slot : InSlotStashTab->GetSlots())
	{
		if (Slot.CanStashAtSlot(InItemCategory, InItemBaseType) == EObsidianPlacingAtSlotResult::CanPlace)
		{
			MatchingSlots.Add(Slot);
		}
	}
	
	return MatchingSlots;
}

UObsidianInventoryItemInstance* FObsidianStashItemList::AddEntry(const TSubclassOf<UObsidianInventoryItemDefinition>& InItemDefClass,
	const FObsidianItemGeneratedData& InItemGeneratedData, const int32 InStackCount, const FObsidianItemPosition& InToPosition)
{
	check(InItemDefClass != nullptr);
	check(OwnerComponent);

	const AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor);

	UObsidianStashTab* StashTab = GetStashTabForTag(InToPosition.GetOwningStashTabTag());
	if(StashTab == nullptr)
	{
		UE_LOG(ObLogPlayerStash, Error, TEXT("StashTab for provided tag is invalid in [%hs]"), __FUNCTION__);
		return nullptr;
	}

#if !UE_BUILD_SHIPPING
	if(StashTab->DebugVerifyPositionFree(InToPosition) == false)
	{
		FFrame::KismetExecutionMessage(*FString::Printf(TEXT("Provided Available Position [x: %d, y: %d] already"
			 "exist in the StashTab's Map in [%hs]"), InToPosition.GetItemGridPosition().X, InToPosition.GetItemGridPosition().Y, __FUNCTION__),
			 ELogVerbosity::Error);
	}
#endif

	FObsidianStashEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.Instance = UObsidianItemsFunctionLibrary::CreateItemInstanceFromDefinition(OwnerComponent->GetOwner(), InItemDefClass,
		InItemGeneratedData, InToPosition);
	NewEntry.OwningStashTab = StashTab;
	NewEntry.StackCount = InStackCount;
	NewEntry.ItemPosition = InToPosition;
	
	UObsidianInventoryItemInstance* Item = NewEntry.Instance;
	
	StashTab->MarkSpaceInTab(Item, InToPosition);
	MarkItemDirty(NewEntry);
	
	BroadcastChangeMessage(NewEntry, /* Old Count */ 0, /* New Count */ NewEntry.StackCount, InToPosition, EObsidianStashChangeType::ICT_ItemAdded);
	return Item;
}

void FObsidianStashItemList::AddEntry(UObsidianInventoryItemInstance* InInstance, const FObsidianItemPosition& InToPosition)
{
	check(InInstance != nullptr);
	check(OwnerComponent);

	UObsidianStashTab* StashTab = GetStashTabForTag(InToPosition.GetOwningStashTabTag());
	if(StashTab == nullptr)
	{
		UE_LOG(ObLogPlayerStash, Error, TEXT("StashTab for provided tag is invalid in [%hs]"), __FUNCTION__);
		return;
	}

#if !UE_BUILD_SHIPPING
	if(StashTab->DebugVerifyPositionFree(InToPosition) == false)
	{
		FFrame::KismetExecutionMessage(*FString::Printf(TEXT("Provided Available Position [x: %d, y: %d] already"
			 "exist in the StashTab's Map in [%hs]"), InToPosition.GetItemGridPosition().X, InToPosition.GetItemGridPosition().Y, __FUNCTION__),
			 ELogVerbosity::Error);
	}
#endif

	FObsidianStashEntry& NewEntry = Entries.Emplace_GetRef(InInstance);
	NewEntry.ItemPosition = InToPosition;
	NewEntry.OwningStashTab = StashTab;
	NewEntry.Instance->SetItemCurrentPosition(InToPosition);
	NewEntry.StackCount = InInstance->GetItemStackCount(ObsidianGameplayTags::Item::StackCount::Current);
	
	StashTab->MarkSpaceInTab(InInstance, InToPosition);
	MarkItemDirty(NewEntry);
	
	BroadcastChangeMessage(NewEntry, /* Old Count */ 0, /* New Count */ NewEntry.StackCount, InToPosition, EObsidianStashChangeType::ICT_ItemAdded);
}

UObsidianInventoryItemInstance* FObsidianStashItemList::LoadEntry(const FObsidianSavedItem& InEquippedSavedItem)
{
	check(OwnerComponent);
	
	UObsidianInventoryItemInstance* LoadedInstance = NewObject<UObsidianInventoryItemInstance>(OwnerComponent->GetOwner());
	LoadedInstance->ConstructFromSavedItem(InEquippedSavedItem);

	const FObsidianItemPosition LoadedPosition = LoadedInstance->GetItemCurrentPosition();
	UObsidianStashTab* StashTab = GetStashTabForTag(LoadedPosition.GetOwningStashTabTag());
	if(StashTab == nullptr)
	{
		UE_LOG(ObLogPlayerStash, Error, TEXT("StashTab for provided tag is invalid in [%hs]"), __FUNCTION__);
		return nullptr;
	}
	
	FObsidianStashEntry& NewEntry = Entries.Emplace_GetRef(LoadedInstance);
	NewEntry.ItemPosition = LoadedPosition;
	NewEntry.OwningStashTab = StashTab;
	NewEntry.Instance->SetItemCurrentPosition(LoadedPosition);
	NewEntry.StackCount = LoadedInstance->GetItemStackCount(ObsidianGameplayTags::Item::StackCount::Current);
	
	StashTab->MarkSpaceInTab(LoadedInstance, LoadedPosition);
	MarkItemDirty(NewEntry);
	
	BroadcastChangeMessage(NewEntry, /* Old Count */ 0, /* New Count */ NewEntry.StackCount, LoadedPosition, EObsidianStashChangeType::ICT_ItemAdded);

	return LoadedInstance;
}

void FObsidianStashItemList::RemoveEntry(UObsidianInventoryItemInstance* InInstance, const FGameplayTag& InStashTabTag)
{
	UObsidianStashTab* StashTab = GetStashTabForTag(InStashTabTag);
	if(StashTab == nullptr)
	{
		UE_LOG(ObLogPlayerStash, Error, TEXT("StashTab for provided tag is invalid in [%hs]"), __FUNCTION__);
		return;
	}
	
	bool bSuccess = false;
	for(auto It = Entries.CreateIterator(); It; ++It)
	{
		FObsidianStashEntry& Entry = *It;
		if(Entry.Instance == InInstance)
		{
			ensure(InStashTabTag == Entry.ItemPosition.GetOwningStashTabTag());
			It.RemoveCurrent();
			MarkArrayDirty();
			bSuccess = true;
		}
	}

	if(bSuccess)
	{
		const FObsidianItemPosition CachedPosition = InInstance->GetItemCurrentPosition();
		InInstance->ResetItemCurrentPosition();
		
		StashTab->UnmarkSpaceInTab(InInstance, CachedPosition);

		const int32 StackCount = InInstance->GetItemStackCount(ObsidianGameplayTags::Item::StackCount::Current);
		BroadcastChangeMessage(InInstance, /* Old Count */ StackCount, /* New Count */ 0, CachedPosition, EObsidianStashChangeType::ICT_ItemRemoved);
		return;
	}
	FFrame::KismetExecutionMessage(TEXT("Provided Instance to remove is not in the Inventory List."), ELogVerbosity::Warning);
}

void FObsidianStashItemList::ChangedEntryStacks(UObsidianInventoryItemInstance* InInstance, const int32 InOldCount, const FGameplayTag& InStashTabTag)
{
	const int32 NewCount = InInstance->GetItemStackCount(ObsidianGameplayTags::Item::StackCount::Current);
	
	bool bSuccess = false;
	for(FObsidianStashEntry& Entry : Entries)
	{
		if(Entry.Instance == InInstance)
		{
			ensure(InStashTabTag == Entry.ItemPosition.GetOwningStashTabTag());
			Entry.StackCount = NewCount;
			MarkItemDirty(Entry);
			bSuccess = true;
		}
	}

	if(bSuccess)
	{
		BroadcastChangeMessage(InInstance, InOldCount, NewCount, InInstance->GetItemCurrentPosition(), EObsidianStashChangeType::ICT_ItemStacksChanged);
		return;
	}
	FFrame::KismetExecutionMessage(TEXT("Provided Instance to change is not in the Inventory List."), ELogVerbosity::Warning);
}

void FObsidianStashItemList::GeneralEntryChange(UObsidianInventoryItemInstance* InInstance, const FGameplayTag& InStashTabTag)
{
	bool bSuccess = false;
	for(FObsidianStashEntry& Entry : Entries)
	{
		if(Entry.Instance == InInstance)
		{
			MarkItemDirty(Entry);
			bSuccess = true;
		}
	}
	
	if(bSuccess)
	{
		const int32 Count = InInstance->GetItemStackCount(ObsidianGameplayTags::Item::StackCount::Current);
		BroadcastChangeMessage(InInstance, Count, Count, InInstance->GetItemCurrentPosition(), EObsidianStashChangeType::ICT_GeneralItemChanged);
		return;
	}
	FFrame::KismetExecutionMessage(TEXT("Provided Instance to change is not in the Inventory List."), ELogVerbosity::Warning);
}

void FObsidianStashItemList::PreReplicatedRemove(const TArrayView<int32> InRemovedIndices, int32 InFinalSize)
{
	for(const int32 Index : InRemovedIndices)
	{
		FObsidianStashEntry& Entry = Entries[Index];
		if (Entry.Instance == nullptr || Entry.LastObservedCount == INDEX_NONE) // Item was never added on this Client.
		{
			continue;
		}

		if (UObsidianStashTab* StashTab = GetStashTabForTag(Entry.ItemPosition.GetOwningStashTabTag()))
		{
			BroadcastChangeMessage(Entry, /* Old Count */ Entry.StackCount, /* New Count */ 0, Entry.ItemPosition, EObsidianStashChangeType::ICT_ItemRemoved);
			Entry.LastObservedCount = 0;
			
			StashTab->UnmarkSpaceInTab(Entry.Instance, Entry.ItemPosition);

			UE_LOG(ObLogPlayerStash, Verbose, TEXT("Replicated removing [%s] item."), *Entry.Instance->GetItemDebugName());
		}
	}
}

void FObsidianStashItemList::PostReplicatedAdd(const TArrayView<int32> InAddedIndices, int32 InFinalSize)
{
	for(const int32 Index : InAddedIndices)
	{
		FObsidianStashEntry& Entry = Entries[Index];
		if (Entry.Instance == nullptr)
		{
			// The Item Instance subobject did not arrive yet, PostReplicatedChange will add the Item once it gets resolved.
			UE_LOG(ObLogPlayerStash, Verbose, TEXT("Replicated Item at index [%d] has no Instance yet, deferring."), Index);
			continue;
		}

		if (UObsidianStashTab* StashTab = GetStashTabForTag(Entry.ItemPosition.GetOwningStashTabTag()))
		{
			BroadcastChangeMessage(Entry, /* Old Count */ 0, /* New Count */ Entry.StackCount, Entry.ItemPosition, EObsidianStashChangeType::ICT_ItemAdded);
			Entry.LastObservedCount = Entry.StackCount;
			
			StashTab->MarkSpaceInTab(Entry.Instance, Entry.ItemPosition);

			UE_LOG(ObLogPlayerStash, Verbose, TEXT("Replicated adding [%s] item."), *Entry.Instance->GetItemDebugName());
		}
	}
}

void FObsidianStashItemList::PostReplicatedChange(const TArrayView<int32> InChangedIndices, int32 InFinalSize)
{
	for(const int32 Index : InChangedIndices)
	{
		FObsidianStashEntry& Entry = Entries[Index];
		if (Entry.Instance == nullptr)
		{
			continue;
		}

		if (Entry.LastObservedCount == INDEX_NONE) // Adding was deferred until the Item Instance got resolved.
		{
			int32 AddedIndex = Index;
			PostReplicatedAdd(MakeArrayView(&AddedIndex, 1), InFinalSize);
			continue;
		}

		if(Entry.LastObservedCount == Entry.StackCount)
		{
			BroadcastChangeMessage(Entry, /* Old Count */ Entry.LastObservedCount, /* New Count */ Entry.StackCount, Entry.ItemPosition, EObsidianStashChangeType::ICT_GeneralItemChanged);
		}
		else
		{
			BroadcastChangeMessage(Entry, /* Old Count */ Entry.LastObservedCount, /* New Count */ Entry.StackCount, Entry.ItemPosition, EObsidianStashChangeType::ICT_ItemStacksChanged);
		}
		Entry.LastObservedCount = Entry.StackCount;

		UE_LOG(ObLogPlayerStash, Verbose, TEXT("Replicated changing [%s] item."), *Entry.Instance->GetItemDebugName());
	}
}

void FObsidianStashItemList::BroadcastChangeMessage(const FObsidianStashEntry& InEntry, const int32 InOldCount, const int32 InNewCount, const FObsidianItemPosition& InItemPosition, const EObsidianStashChangeType& InChangeType) const
{
	FObsidianStashChangeMessage Message;
	Message.PlayerStashOwner = OwnerComponent;
	Message.ItemInstance = InEntry.Instance;
	Message.ItemPosition = InItemPosition;
	Message.NewCount = InNewCount;
	Message.Delta = InNewCount - InOldCount;
	Message.ChangeType = InChangeType;

	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(OwnerComponent->GetWorld());
	MessageSubsystem.BroadcastMessage(ObsidianGameplayTags::Message::PlayerStash::Changed, Message);
}

