// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project
#include "ObsidianTypes/ObsidianItemTypes.h"

#include "Net/Serialization/FastArraySerializer.h"
#include "ObsidianStashItemList.generated.h"

class UObsidianStashTabsConfig;
class UObsidianStashTab;
struct FObsidianStashItemList;

class UObsidianPlayerStashComponent;
class UObsidianInventoryItemInstance;
class UObsidianInventoryItemDefinition;

/**
 * 
 */
UENUM(BlueprintType)
enum class EObsidianStashChangeType : uint8
{
	ICT_NONE = 0 UMETA(DisplayName = "None"),
	ICT_ItemRemoved UMETA(DisplayName = "Item Removed"),
	ICT_ItemAdded UMETA(DisplayName = "Item Added"),
	ICT_ItemStacksChanged UMETA(DisplayName = "Item Stacks Changed"),
	ICT_GeneralItemChanged UMETA(DisplayName = "General Item Changed"),

	ICT_MAX
};

/**
 * A single entry in a Player Stash.
 */
USTRUCT(BlueprintType)
struct FObsidianStashEntry : public FFastArraySerializerItem
{
	GENERATED_BODY();
	
	FObsidianStashEntry()
		: Instance(nullptr)
	{}
	FObsidianStashEntry(UObsidianInventoryItemInstance* InInstance)
		: Instance(InInstance)
	{}

	FString GetDebugString() const;

private:
	friend FObsidianStashItemList;
	friend UObsidianPlayerStashComponent;

	UPROPERTY()
	TObjectPtr<UObsidianInventoryItemInstance> Instance;

	UPROPERTY()
	int32 StackCount = 0;

	UPROPERTY(NotReplicated)
	int32 LastObservedCount = INDEX_NONE;

	UPROPERTY()
	FObsidianItemPosition ItemPosition = FObsidianItemPosition();
};

/**
 * List of inventory items.
 */
USTRUCT(BlueprintType)
struct FObsidianStashItemList : public FFastArraySerializer
{
	GENERATED_BODY();
	
public:
	FObsidianStashItemList()
		: OwnerComponent(nullptr)
	{}
	FObsidianStashItemList(UActorComponent* InOwnerComponent)
		: OwnerComponent(InOwnerComponent)
	{}

	void InitializeStashTabs(const UObsidianStashTabsConfig* StashTabsConfig);

	TArray<UObsidianInventoryItemInstance*> GetAllItems() const;
	int32 GetEntriesCount() const;

	UObsidianInventoryItemInstance* AddEntry(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDefClass, const int32 StackCount, const FObsidianItemPosition& ToPosition);
	void AddEntry(UObsidianInventoryItemInstance* Instance, const FObsidianItemPosition& ToPosition);
	void RemoveEntry(UObsidianInventoryItemInstance* Instance);
	void ChangedEntryStacks(UObsidianInventoryItemInstance* Instance, const int32 OldCount);
	void GeneralEntryChange(UObsidianInventoryItemInstance* Instance);

	/** Marks Item space in the internal Inventory State map. Must be called after adding new item. */
	void Item_MarkSpace(const UObsidianInventoryItemInstance* ItemInstance, const FObsidianItemPosition& AtPosition);
	
	/** Unmarks Item space in the internal Inventory State map. Must be called after removing item. */
	void Item_UnMarkSpace(const UObsidianInventoryItemInstance* ItemInstance, const FObsidianItemPosition& AtPosition);

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FObsidianStashEntry, FObsidianStashItemList>(Entries, DeltaParams, *this);
	}

	//~ Start of FFastArraySerializer contract
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
	//~ End of FFastArraySerializer contract

private:
	void BroadcastChangeMessage(const FObsidianStashEntry& Entry, const int32 OldCount, const int32 NewCount, const FObsidianItemPosition& ItemPosition, const EObsidianStashChangeType& ChangeType) const;
	
private:
	friend UObsidianPlayerStashComponent;

	/** Replicated list of all items. */
	UPROPERTY()
	TArray<FObsidianStashEntry> Entries;
	
	UPROPERTY(NotReplicated)
	TObjectPtr<UActorComponent> OwnerComponent;
	
	TArray<TObjectPtr<UObsidianStashTab>> StashTabs;
};

template<>
struct TStructOpsTypeTraits<FObsidianStashItemList> : public TStructOpsTypeTraitsBase2<FObsidianStashItemList>
{
	enum
	{
		WithNetDeltaSerializer = true
	};
};