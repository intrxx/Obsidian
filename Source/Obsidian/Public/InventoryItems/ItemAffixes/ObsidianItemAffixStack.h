// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Net/Serialization/FastArraySerializer.h"

#include "ObsidianTypes/ItemTypes/ObsidianItemTypes.h"

#include "ObsidianItemAffixStack.generated.h"

struct FObsidianDynamicItemAffix;
struct FObsidianItemAffixStack;

class UObsidianInventoryItemInstance;

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
	int32 GetPrefixAndSuffixCount() const;
	int32 GetPrefixCount() const;
	int32 GetSuffixCount() const;

	bool HasImplicit() const;
	bool HasSkillImplicit() const;

	TArray<FObsidianActiveItemAffix> GetAllItemAffixes() const;
	TArray<FObsidianActiveItemAffix> GetAllItemPrefixesAndSuffixes() const;
	
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& InOutDeltaParams)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FObsidianAffixEntry, FObsidianItemAffixStack>(Entries, InOutDeltaParams, *this);
	}

	void InitializeAffixes(UObsidianInventoryItemInstance* InOwningInstance, const TArray<FObsidianActiveItemAffix>& InAffixesToInitialize);
	void AddAffix(UObsidianInventoryItemInstance* InOwningInstance, const FObsidianActiveItemAffix& InItemAffix);
	bool RemoveAffix(const FGameplayTag& InAffixTag);
	bool RemoveSkillImplicitAffix();
	bool RemoveAllPrefixesAndSuffixes();
	void AffixChanged(const FGameplayTag& InAffixTag);
	
	//~ Start of FFastArraySerializer contract
	void PreReplicatedRemove(const TArrayView<int32> InRemovedIndices, int32 InFinalSize);
	void PostReplicatedAdd(const TArrayView<int32> InAddedIndices, int32 InFinalSize);
	void PostReplicatedChange(const TArrayView<int32> InChangedIndices, int32 InFinalSize);
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
