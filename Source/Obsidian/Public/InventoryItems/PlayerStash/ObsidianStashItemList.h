// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project
#include "ObsidianTypes/ItemTypes/ObsidianItemTypes.h"

#include "Net/Serialization/FastArraySerializer.h"
#include "ObsidianStashItemList.generated.h"

class UObsidianStashTab_Slots;
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
 * 
 */
USTRUCT(BlueprintType)
struct FObsidianStashChangeMessage
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly, Category = "Obsidian|StashTab")
	TObjectPtr<UActorComponent> PlayerStashOwner = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Obsidian|StashTab")
	TObjectPtr<UObsidianInventoryItemInstance> ItemInstance = nullptr;
	
	UPROPERTY(BlueprintReadOnly, Category = "Obsidian|StashTab")
	FObsidianItemPosition ItemPosition = FObsidianItemPosition();

	UPROPERTY(BlueprintReadOnly, Category = "Obsidian|StashTab")
	int32 NewCount = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Obsidian|StashTab")
	int32 Delta = 0;
	
	UPROPERTY(BlueprintReadOnly, Category = "Obsidian|StashTab")
	EObsidianStashChangeType ChangeType = EObsidianStashChangeType::ICT_NONE;
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

	TArray<UObsidianStashTab*> InitializeStashTabs(const UObsidianStashTabsConfig* StashTabsConfig);

	TArray<UObsidianInventoryItemInstance*> GetAllItems() const;
	TArray<UObsidianInventoryItemInstance*> GetAllItemsFromStashTab(const FGameplayTag& StashTabTag);
	int32 GetEntriesCount() const;
	UObsidianStashTab* GetStashTabForTag(const FGameplayTag& StashTabTag);

	TArray<FObsidianSlotDefinition> FindMatchingSlotsForItemCategory(const FGameplayTag& ItemCategory, const UObsidianStashTab_Slots* SlotStashTab);

	UObsidianInventoryItemInstance* AddEntry(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDefClass, const int32 StackCount, const FObsidianItemPosition& ToPosition);
	void AddEntry(UObsidianInventoryItemInstance* Instance, const FObsidianItemPosition& ToPosition);
	void RemoveEntry(UObsidianInventoryItemInstance* Instance, const FGameplayTag& StashTabTag);
	void ChangedEntryStacks(UObsidianInventoryItemInstance* Instance, const int32 OldCount, const FGameplayTag& StashTabTag);
	void GeneralEntryChange(UObsidianInventoryItemInstance* Instance, const FGameplayTag& StashTabTag);

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

#if WITH_GAMEPLAY_DEBUGGER
	friend class FGameplayDebuggerCategory_PlayerStash;
#endif

	/** Replicated list of all items. */
	UPROPERTY()
	TArray<FObsidianStashEntry> Entries;
	
	UPROPERTY(NotReplicated)
	TObjectPtr<UActorComponent> OwnerComponent;
	
	TMap<FGameplayTag, UObsidianStashTab*> StashTabsMap;
};

template<>
struct TStructOpsTypeTraits<FObsidianStashItemList> : public TStructOpsTypeTraitsBase2<FObsidianStashItemList>
{
	enum
	{
		WithNetDeltaSerializer = true
	};
};