// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "InventoryItems/Fragments/OInventoryItemFragment_Affixes.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "ObsidianItemAffixStack.generated.h"

class UObsidianInventoryItemInstance;
struct FObsidianItemAffix;
struct FObsidianItemAffixStack;

/**
 *	A single Entry in Item affix Stack.
 */
USTRUCT(BlueprintType)
struct FObsidianAffixEntry : public FFastArraySerializerItem
{
	GENERATED_BODY();

public:
	FObsidianAffixEntry()
	{}
	FObsidianAffixEntry(const FObsidianItemAffix& Affix)
		: AffixIdentifierTag(Affix.AffixTag)
		, AffixType(Affix.AffixType)
		, AffixTier(Affix.AffixTier)
		, AffixDescription(Affix.AffixDescription)
		, TempAffixMagnitude(Affix.TempAffixMagnitude)
	{}
	
private:
	friend FObsidianItemAffixStack;
	
	UPROPERTY()
	FGameplayTag AffixIdentifierTag = FGameplayTag::EmptyTag;
	
	UPROPERTY()
	EObsidianAffixType AffixType = EObsidianAffixType::None;

	UPROPERTY()
	int32 AffixTier = -1;

	UPROPERTY()
	FText AffixDescription = FText();

	/**TODO just a simple int for now, will cover more later. */
	UPROPERTY()
	int32 TempAffixMagnitude = 0;
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
	
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FObsidianAffixEntry, FObsidianItemAffixStack>(Entries, DeltaParams, *this);
	}

	void AddAffix(const FObsidianItemAffix& ItemAffix);
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
