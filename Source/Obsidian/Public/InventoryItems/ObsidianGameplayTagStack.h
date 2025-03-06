// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "ObsidianGameplayTagStack.generated.h"

class UObsidianInventoryItemInstance;
struct FGameplayTagStackContainer;
struct FNetDeltaSerializeInfo;

/**
 * Represents one stack of a gameplay tag (tag + count).
 */
USTRUCT(BlueprintType)
struct FGameplayTagStack : public FFastArraySerializerItem
{
	GENERATED_BODY();

	FGameplayTagStack(){}
	FGameplayTagStack(const FGameplayTag InTag, const int32 InStackCount)
		: Tag(InTag)
		, StackCount(InStackCount)
	{}

	FString GetDebugString() const;
	
private:
	friend FGameplayTagStackContainer;
	
	UPROPERTY()
	FGameplayTag Tag;

	UPROPERTY()
	int32 StackCount = 0;
};

/**
 *	Container of gameplay tag stack.
 */
USTRUCT(BlueprintType)
struct FGameplayTagStackContainer : public FFastArraySerializer
{
	GENERATED_BODY();

	FGameplayTagStackContainer(){}

public:
	/** Adds a specified number of stacks to the provided tag (does nothing if StackCount is below 1). */
	void AddStack(FGameplayTag ToTag, int32 StackCount);

	/** Removes a specified number of stacks to the provided tag (does nothing if StackCount is below 1). */
	void RemoveStack(FGameplayTag FromTag, int32 StackCount);
	
	/** Overrides stacks on provided tag (does nothing if StackCount is below 1). */
	void OverrideStack(FGameplayTag Tag, int32 NewStackCount);

	/** Returns the stack count of the specified tag (or 0 if the tag is not present). */
	int32 GetStackCount(const FGameplayTag Tag) const
	{
		return TagToCountMap.FindRef(Tag);
	}

	/** Returns true if there is at least one stack of the specified tag. */
	bool ContainsTag(const FGameplayTag Tag) const
	{
		return TagToCountMap.Contains(Tag);
	}

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FGameplayTagStack, FGameplayTagStackContainer>(Stacks, DeltaParams, *this);
	}

	//~ Start of FFastArraySerializer contract
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
	//~ End of FFastArraySerializer contract

private:
	friend UObsidianInventoryItemInstance;
	
	/** Replicated list of gameplay tag stacks. */
	UPROPERTY()
	TArray<FGameplayTagStack> Stacks;

	/** Accelerated list of tag stacks for queries. */
	TMap<FGameplayTag, int32> TagToCountMap;
};

template<>
struct TStructOpsTypeTraits<FGameplayTagStackContainer> : public TStructOpsTypeTraitsBase2<FGameplayTagStackContainer>
{
	enum
	{
		WithNetDeltaSerializer = true,
	};
};
