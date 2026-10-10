// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"
#include "Net/Serialization/FastArraySerializer.h"

#include "ObsidianTypes/ItemTypes/ObsidianItemTypes.h"

#include "ObsidianStashItemList.generated.h"

struct FObsidianStashItemList;
struct FObsidianItemGeneratedData;
struct FObsidianSavedItem;

class UObsidianStashTab_Slots;
class UObsidianStashTabsConfig;
class UObsidianStashTab;
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
 * 
 */
USTRUCT(BlueprintType)
struct FObsidianStashSlotDefinition
{
	GENERATED_BODY()

public:
	FObsidianStashSlotDefinition(){}

	bool IsValid() const;
	bool HasLimitedStacks() const;
	FGameplayTag GetStashSlotTag() const;
	
	EObsidianPlacingAtSlotResult CanStashAtSlot(const FGameplayTag& InItemCategory, const FGameplayTag& InItemBaseType) const;

	void AddBannedStashCategory(const FGameplayTag& InBannedCategory);
	void AddBannedStashCategories(const FGameplayTagContainer& InBannedCategories);
	void RemoveBannedStashCategory(const FGameplayTag& InBannedCategoryToRemove);
	void RemoveBannedStashCategories(const FGameplayTagContainer& InBannedCategoriesToRemove);

public:
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	FObsidianSlotDefinition BaseSlotDefinition = FObsidianSlotDefinition();
	
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	bool bRequireUniqueBaseTypeMatch = false;
	
	/** Tag of this type is required on item if bRequireUniqueBaseTypeMatch is checked. */
	UPROPERTY(EditDefaultsOnly, Meta = (EditCondition = "bRequireUniqueBaseTypeMatch", Categories = "Item.BaseType"), Category = "Obsidian")
	FGameplayTag UniqueBaseTypeTag = FGameplayTag::EmptyTag;

	/** Amount of Stacks of provided Item the Slot can store. Leave at INDEX_NONE for unlimited storage, this is the default behaviour. */
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	int32 SlotStackLimit = INDEX_NONE;
	
	static const FObsidianStashSlotDefinition InvalidSlot;
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

	UPROPERTY()
	UObsidianStashTab* OwningStashTab = nullptr;
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

	TArray<UObsidianStashTab*> InitializeStashTabs(const UObsidianStashTabsConfig* InStashTabsConfig);

	TArray<UObsidianInventoryItemInstance*> GetAllItems() const;
	TArray<UObsidianInventoryItemInstance*> GetAllPersonalItems() const;
	TArray<UObsidianInventoryItemInstance*> GetAllSharedItems() const;
	TArray<UObsidianInventoryItemInstance*> GetAllItemsFromStashTab(const FGameplayTag& InStashTabTag);
	int32 GetEntriesCount() const;
	UObsidianStashTab* GetStashTabForTag(const FGameplayTag& InStashTabTag);

	TArray<FObsidianStashSlotDefinition> FindMatchingSlotsForItemCategory(const FGameplayTag& InItemCategory, const FGameplayTag& InItemBaseType, const UObsidianStashTab_Slots* InSlotStashTab);

	UObsidianInventoryItemInstance* AddEntry(const TSubclassOf<UObsidianInventoryItemDefinition>& InItemDefClass, const FObsidianItemGeneratedData& InItemGeneratedData,
		const int32 InStackCount, const FObsidianItemPosition& InToPosition);
	void AddEntry(UObsidianInventoryItemInstance* InInstance, const FObsidianItemPosition& InToPosition);
	UObsidianInventoryItemInstance* LoadEntry(const FObsidianSavedItem& InEquippedSavedItem);
	void RemoveEntry(UObsidianInventoryItemInstance* InInstance, const FGameplayTag& InStashTabTag);
	void ChangedEntryStacks(UObsidianInventoryItemInstance* InInstance, const int32 InOldCount, const FGameplayTag& InStashTabTag);
	void GeneralEntryChange(UObsidianInventoryItemInstance* InInstance, const FGameplayTag& InStashTabTag);

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& InOutDeltaParams)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FObsidianStashEntry, FObsidianStashItemList>(Entries, InOutDeltaParams, *this);
	}

	//~ Start of FFastArraySerializer contract
	void PreReplicatedRemove(const TArrayView<int32> InRemovedIndices, int32 InFinalSize);
	void PostReplicatedAdd(const TArrayView<int32> InAddedIndices, int32 InFinalSize);
	void PostReplicatedChange(const TArrayView<int32> InChangedIndices, int32 InFinalSize);
	//~ End of FFastArraySerializer contract

private:
	void BroadcastChangeMessage(const FObsidianStashEntry& InEntry, const int32 InOldCount, const int32 InNewCount, const FObsidianItemPosition& InItemPosition, const EObsidianStashChangeType& InChangeType) const;
	
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