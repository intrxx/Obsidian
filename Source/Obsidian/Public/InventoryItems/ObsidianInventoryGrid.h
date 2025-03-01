// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "ObsidianInventoryGrid.generated.h"

class UObsidianInventoryItemDefinition;
class UObsidianInventoryItemInstance;
class UObsidianInventoryComponent;
struct FObsidianInventoryGrid;

UENUM(BlueprintType)
enum class EObsidianInventoryChangeType : uint8
{
	ICT_NONE = 0 UMETA(DisplayName = "None"),
	ICT_ItemRemoved UMETA(DisplayName = "Item Removed"),
	ICT_ItemAdded UMETA(DisplayName = "Item Added"),
	ICT_ItemChanged UMETA(DisplayName = "Item Changed"),

	ICT_MAX
};

USTRUCT(BlueprintType)
struct FObsidianInventoryChangeMessage
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly, Category = "Obsidian|Inventory")
	TObjectPtr<UActorComponent> InventoryOwner = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Obsidian|Inventory")
	TObjectPtr<UObsidianInventoryItemInstance> ItemInstance = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Obsidian|Inventory")
	FVector2D GridItemPosition = FVector2d::Zero();

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
	friend FObsidianInventoryGrid;
	friend UObsidianInventoryComponent;

	UPROPERTY()
	TObjectPtr<UObsidianInventoryItemInstance> Instance;

	UPROPERTY()
	int32 StackCount = 0;

	UPROPERTY(NotReplicated)
	int32 LastObservedCount = INDEX_NONE;

	UPROPERTY()
	FVector2D GridLocation = FVector2D::Zero();
};

/**
 * List of inventory items.
 */
USTRUCT(BlueprintType)
struct FObsidianInventoryGrid : public FFastArraySerializer
{
	GENERATED_BODY();
	
public:
	FObsidianInventoryGrid()
		: OwnerComponent(nullptr)
	{}

	FObsidianInventoryGrid(UActorComponent* InOwnerComponent)
		: OwnerComponent(InOwnerComponent)
	{}

	TArray<UObsidianInventoryItemInstance*> GetAllItems() const;
	int32 GetEntriesCount() const;

	UObsidianInventoryItemInstance* AddEntry(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDefClass, const int32 StackCount, const FVector2D& AvailablePosition);
	void AddEntry(UObsidianInventoryItemInstance* Instance, const FVector2D& AvailablePosition);
	void RemoveEntry(UObsidianInventoryItemInstance* Instance);
	void ChangedEntryStacks(UObsidianInventoryItemInstance* Instance, const int32 OldCount);

	/** Marks Item space in the internal Inventory State map. Must be called after adding new item. */
	void Item_MarkSpace(const UObsidianInventoryItemInstance* ItemInstance, const FVector2D AtPosition);
	
	/** Unmarks Item space in the internal Inventory State map. Must be called after removing item. */
	void Item_UnMarkSpace(const UObsidianInventoryItemInstance* ItemInstance, const FVector2D AtPosition);

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FObsidianInventoryEntry, FObsidianInventoryGrid>(Entries, DeltaParams, *this);
	}

	//~ Start of FFastArraySerializer contract
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
	//~ End of FFastArraySerializer contract

private:
	void BroadcastChangeMessage(const FObsidianInventoryEntry& Entry, const int32 OldCount, const int32 NewCount, const FVector2D& GridPosition, const EObsidianInventoryChangeType& ChangeType) const;
	
private:
	friend UObsidianInventoryComponent;

	/** Replicated list of all items. */
	UPROPERTY()
	TArray<FObsidianInventoryEntry> Entries;
	
	UPROPERTY(NotReplicated)
	TObjectPtr<UActorComponent> OwnerComponent;

	/** Accelerated list of item location (0, 0). */
	TMap<FVector2D, UObsidianInventoryItemInstance*> GridLocationToItemMap;

	/**
	 * Map that represents whole Inventory Grid with taken fields.
	 * If a Given Vector2D location has a true value associated with it, the field is treated as taken.
	 */
	TMap<FVector2D, bool> InventoryStateMap;
};

template<>
struct TStructOpsTypeTraits<FObsidianInventoryGrid> : public TStructOpsTypeTraitsBase2<FObsidianInventoryGrid>
{
	enum
	{
		WithNetDeltaSerializer = true
	};
};