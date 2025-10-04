// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

// ~ Project
#include "ObsidianTypes/ObsidianItemTypes.h"

#include "Net/Serialization/FastArraySerializer.h"
#include "ObsidianItemAffixStack.generated.h"

struct FObsidianDynamicItemAffix;
struct FObsidianItemAffixStack;

class UObsidianInventoryItemInstance;

DECLARE_LOG_CATEGORY_EXTERN(LogAffixes, Log, All);

/**
 *	A single Entry in Item affix Stack.
 */
USTRUCT(BlueprintType)
struct FObsidianAffixEntry : public FFastArraySerializerItem
{
	GENERATED_BODY();

public:
	FObsidianAffixEntry(){}
	FObsidianAffixEntry(const FObsidianActiveItemAffix& InAffix)
		: ActiveItemAffix(InAffix)
	{}

private:
	friend FObsidianItemAffixStack;

	UPROPERTY()
	FObsidianActiveItemAffix ActiveItemAffix = FObsidianActiveItemAffix();
	
	UPROPERTY()
	TObjectPtr<UObsidianInventoryItemInstance> OwningItem = nullptr;
};


/**
 * List of items affixes.
 */
USTRUCT()
struct FObsidianItemAffixStack : public FFastArraySerializer
{
	GENERATED_BODY()
	
public:
	FObsidianItemAffixStack()
		: OwningItem(nullptr)
	{}
	FObsidianItemAffixStack(UObsidianInventoryItemInstance* InOwningItem)
		: OwningItem(InOwningItem)
	{}

	int32 GetTotalAffixCount() const;
	int32 GetPrefixCount() const;
	int32 GetSuffixCount() const;

	bool HasImplicit() const;

	TArray<FObsidianActiveItemAffix> GetAllItemAffixes() const;
	
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FObsidianAffixEntry, FObsidianItemAffixStack>(Entries, DeltaParams, *this);
	}

	void InitializeAffixes(UObsidianInventoryItemInstance* InOwningInstance, const TArray<FObsidianActiveItemAffix>& AffixesToInitialize);
	void AddAffix(UObsidianInventoryItemInstance* InOwningInstance, const FObsidianActiveItemAffix& ItemAffix);
	void RemoveAffix(const FGameplayTag& AffixTag);
	void AffixChanged(const FGameplayTag& AffixTag);

	//~ Start of FFastArraySerializer contract
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
	//~ End of FFastArraySerializer contract
	
private:
	/** Replicated list of all affixes. */
	UPROPERTY()
	TArray<FObsidianAffixEntry> Entries;

	UPROPERTY()
	TObjectPtr<UObsidianInventoryItemInstance> OwningItem;
	
};

template<>
struct TStructOpsTypeTraits<FObsidianItemAffixStack> : public TStructOpsTypeTraitsBase2<FObsidianItemAffixStack>
{
	enum
	{
		WithNetDeltaSerializer = true
	};
};
