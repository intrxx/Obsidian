// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"
#include "Net/Serialization/FastArraySerializer.h"

#include "ObsidianInventoryGridItemList.generated.h"

struct FObsidianInventoryGridItemList;
struct FObsidianItemGeneratedData;
struct FObsidianSavedItem;

class UObsidianInventoryItemDefinition;
class UObsidianInventoryItemInstance;
class UObsidianInventoryComponent;

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

public:
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
	FObsidianInventoryEntry(UObsidianInventoryItemInstance* InInstance, const FIntPoint& InGridLocation)
		: Instance(InInstance)
		, GridLocation(InGridLocation)
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

	UObsidianInventoryItemInstance* AddEntry(const TSubclassOf<UObsidianInventoryItemDefinition>& InItemDefClass, const FObsidianItemGeneratedData& InItemGeneratedData,
		const int32 InStackCount, const FIntPoint& InAvailablePosition);
	void AddEntry(UObsidianInventoryItemInstance* InInstance, const FIntPoint& InAvailablePosition);
	UObsidianInventoryItemInstance* LoadEntry(const FObsidianSavedItem& InEquippedSavedItem);
	void RemoveEntry(UObsidianInventoryItemInstance* InInstance);
	void ChangedEntryStacks(UObsidianInventoryItemInstance* InInstance, const int32 InOldCount);
	void GeneralEntryChange(UObsidianInventoryItemInstance* InInstance);

	/** Marks Item space in the internal Inventory State map. Must be called after adding new item. */
	void Item_MarkSpace(const UObsidianInventoryItemInstance* InItemInstance, const FIntPoint& InAtPosition);
	
	/** Unmarks Item space in the internal Inventory State map. Must be called after removing item. */
	void Item_UnMarkSpace(const UObsidianInventoryItemInstance* InItemInstance, const FIntPoint& InAtPosition);

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& InOutDeltaParams)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FObsidianInventoryEntry, FObsidianInventoryGridItemList>(Entries, InOutDeltaParams, *this);
	}

	//~ Start of FFastArraySerializer contract
	void PreReplicatedRemove(const TArrayView<int32> InRemovedIndices, int32 InFinalSize);
	void PostReplicatedAdd(const TArrayView<int32> InAddedIndices, int32 InFinalSize);
	void PostReplicatedChange(const TArrayView<int32> InChangedIndices, int32 InFinalSize);
	//~ End of FFastArraySerializer contract

private:
	void BroadcastChangeMessage(const FObsidianInventoryEntry& InEntry, const int32 InOldCount, const int32 InNewCount, const FIntPoint& InGridPosition, const EObsidianInventoryChangeType& InChangeType) const;
	
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