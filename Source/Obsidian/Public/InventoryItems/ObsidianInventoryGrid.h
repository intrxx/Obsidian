// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "ObsidianInventoryGrid.generated.h"

class UObsidianInventoryItemDefinition;
class UObsidianInventoryItemInstance;
class UObsidianInventoryComponent;
struct FObsidianInventoryGrid;

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

	UPROPERTY()
	int32 LastObservedCount = INDEX_NONE;
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
	friend UObsidianInventoryComponent;

	/** Replicated list of all items. */
	UPROPERTY()
	TArray<FObsidianInventoryEntry> Entries;
	
	UPROPERTY(NotReplicated)
	TObjectPtr<UActorComponent> OwnerComponent;

	/** Accelerated list of item location (0, 0). */
	TMap<FVector2D, UObsidianInventoryItemInstance*> GridLocationToItemMap;
};

template<>
struct TStructOpsTypeTraits<FObsidianInventoryGrid> : public TStructOpsTypeTraitsBase2<FObsidianInventoryGrid>
{
	enum
	{
		WithNetDeltaSerializer = true
	};
};