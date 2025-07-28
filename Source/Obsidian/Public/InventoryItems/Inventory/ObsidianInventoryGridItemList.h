// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project


#include "Net/Serialization/FastArraySerializer.h"
#include "ObsidianInventoryGridItemList.generated.h"

class UObsidianInventoryItemDefinition;
class UObsidianInventoryItemInstance;
class UObsidianInventoryComponent;
struct FObsidianInventoryGridItemList;

/**
 * 
 */
UENUM(BlueprintType)
enum class EObsidianInventoryChangeType : uint8
{
	ICT_NONE = 0 UMETA(DisplayName = "None"),
	ICT_ItemRemoved UMETA(DisplayName = "Item Removed"),
	ICT_ItemAdded UMETA(DisplayName = "Item Added"),
	ICT_ItemStacksChanged UMETA(DisplayName = "Item Stacks Changed"),
	ICT_GeneralItemChanged UMETA(DisplayName = "General Item Changed"),

	ICT_MAX
};

/**
 * 
 */
USTRUCT(BlueprintType)
struct FObsidianInventoryChangeMessage
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly, Category = "Obsidian|Inventory")
	TObjectPtr<UActorComponent> InventoryOwner = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Obsidian|Inventory")
	TObjectPtr<UObsidianInventoryItemInstance> ItemInstance = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Obsidian|Inventory")
	FIntPoint GridItemPosition = FIntPoint::NoneValue;

	UPROPERTY(BlueprintReadOnly, Category = "Obsidian|Inventory")
	int32 NewCount = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Obsidian|Inventory")
	int32 Delta = 0;
	
	//TODO Decide if I want to actually use it, it shouldn't be an issue but the Change type can be deduced from Stacks changes so it is kinda useless to the actual game logic
	// InventoryChangeMessage.NewCount == 0 -> Removed
	// InventoryChangeMessage.NewCount == InventoryChangeMessage.Delta -> Added
	// InventoryChangeMessage.NewCount != InventoryChangeMessage.Delta -> Changed
	UPROPERTY(BlueprintReadOnly, Category = "Obsidian|Inventory")
	EObsidianInventoryChangeType ChangeType = EObsidianInventoryChangeType::ICT_NONE;
};

/**
 * A single entry in an inventory.
 */
USTRUCT(BlueprintType)
struct FObsidianInventoryEntry : public FFastArraySerializerItem
{
	GENERATED_BODY();
	
	FObsidianInventoryEntry()
		: Instance(nullptr)
	{}
	FObsidianInventoryEntry(UObsidianInventoryItemInstance* InInstance)
		: Instance(InInstance)
	{}

	FString GetDebugString() const;

private:
	friend FObsidianInventoryGridItemList;
	friend UObsidianInventoryComponent;

	UPROPERTY()
	TObjectPtr<UObsidianInventoryItemInstance> Instance;

	UPROPERTY()
	int32 StackCount = 0;

	UPROPERTY(NotReplicated)
	int32 LastObservedCount = INDEX_NONE;

	UPROPERTY()
	FIntPoint GridLocation = FIntPoint::NoneValue;
};

/**
 * List of inventory items.
 */
USTRUCT(BlueprintType)
struct FObsidianInventoryGridItemList : public FFastArraySerializer
{
	GENERATED_BODY();
	
public:
	FObsidianInventoryGridItemList()
		: OwnerComponent(nullptr)
	{}
	FObsidianInventoryGridItemList(UActorComponent* InOwnerComponent)
		: OwnerComponent(InOwnerComponent)
	{}

	TArray<UObsidianInventoryItemInstance*> GetAllItems() const;
	int32 GetEntriesCount() const;

	UObsidianInventoryItemInstance* AddEntry(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDefClass, const int32 StackCount, const FIntPoint& AvailablePosition);
	void AddEntry(UObsidianInventoryItemInstance* Instance, const FIntPoint& AvailablePosition);
	void RemoveEntry(UObsidianInventoryItemInstance* Instance);
	void ChangedEntryStacks(UObsidianInventoryItemInstance* Instance, const int32 OldCount);
	void GeneralEntryChange(UObsidianInventoryItemInstance* Instance);

	/** Marks Item space in the internal Inventory State map. Must be called after adding new item. */
	void Item_MarkSpace(const UObsidianInventoryItemInstance* ItemInstance, const FIntPoint& AtPosition);
	
	/** Unmarks Item space in the internal Inventory State map. Must be called after removing item. */
	void Item_UnMarkSpace(const UObsidianInventoryItemInstance* ItemInstance, const FIntPoint& AtPosition);

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FObsidianInventoryEntry, FObsidianInventoryGridItemList>(Entries, DeltaParams, *this);
	}

	//~ Start of FFastArraySerializer contract
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
	//~ End of FFastArraySerializer contract

private:
	void BroadcastChangeMessage(const FObsidianInventoryEntry& Entry, const int32 OldCount, const int32 NewCount, const FIntPoint& GridPosition, const EObsidianInventoryChangeType& ChangeType) const;
	
private:
	friend UObsidianInventoryComponent;

	/** Replicated list of all items. */
	UPROPERTY()
	TArray<FObsidianInventoryEntry> Entries;
	
	UPROPERTY(NotReplicated)
	TObjectPtr<UActorComponent> OwnerComponent;

	/** Accelerated list of item location (0, 0). */
	TMap<FIntPoint, UObsidianInventoryItemInstance*> GridLocationToItemMap;

	/**
	 * Map that represents whole Inventory Grid with taken fields.
	 * If a Given FIntPoint location has a true value associated with it, the field is treated as taken.
	 */
	TMap<FIntPoint, bool> InventoryStateMap;
};

template<>
struct TStructOpsTypeTraits<FObsidianInventoryGridItemList> : public TStructOpsTypeTraitsBase2<FObsidianInventoryGridItemList>
{
	enum
	{
		WithNetDeltaSerializer = true
	};
};