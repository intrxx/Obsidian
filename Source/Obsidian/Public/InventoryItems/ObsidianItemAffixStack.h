// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "ObsidianItemAffixStack.generated.h"

/**
 *	A single Entry in Item affix Stack.
 */
USTRUCT(BlueprintType)
struct FObsidianAffixEntry : public FFastArraySerializerItem
{
	GENERATED_BODY();
	
};


/**
 * List of items affixes.
 */
USTRUCT()
struct FObsidianItemAffixStack : public FFastArraySerializer
{
	GENERATED_BODY()
	
public:
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FObsidianAffixEntry, FObsidianItemAffixStack>(Entries, DeltaParams, *this);
	}

	//~ Start of FFastArraySerializer contract
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
	//~ End of FFastArraySerializer contract
	
private:
	/** Replicated list of all items. */
	UPROPERTY()
	TArray<FObsidianAffixEntry> Entries;
	
};

template<>
struct TStructOpsTypeTraits<FObsidianItemAffixStack> : public TStructOpsTypeTraitsBase2<FObsidianItemAffixStack>
{
	enum
	{
		WithNetDeltaSerializer = true
	};
};
