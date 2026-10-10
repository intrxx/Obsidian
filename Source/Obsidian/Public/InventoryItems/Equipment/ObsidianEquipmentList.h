// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Net/Serialization/FastArraySerializer.h"

#include "InventoryItems/ItemAffixes/ObsidianAffixAbilitySet.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "ObsidianTypes/ItemTypes/ObsidianItemTypes.h"

#include "ObsidianEquipmentList.generated.h"

struct FObsidianSavedItem;
struct FObsidianEquipmentList;
struct FObsidianItemGeneratedData;

class AObsidianHero;
class UObsidianAffixAbilitySet;
class UObsidianAbilitySystemComponent;
class UObsidianInventoryItemInstance;
class UObsidianInventoryItemDefinition;
class UObsidianEquipmentComponent;

/**
 * 
 */
UENUM(BlueprintType)
enum class EObsidianEquipmentChangeType : uint8
{
	ECT_None = 0 UMETA(DisplayName = "None"),
	ECT_ItemUnequipped UMETA(DisplayName = "Item Unequipped"),
	ECT_ItemEquipped UMETA(DisplayName = "Item Equipped"),
	ECT_ItemSwapped UMETA(DisplayName = "Item Swapped"),

	ECT_MAX
};

/**
 * 
 */
USTRUCT(BlueprintType)
struct FObsidianEquipmentChangeMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Obsidian|Equipement")
	TObjectPtr<UActorComponent> EquipmentOwner = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Obsidian|Equipement")
	TObjectPtr<UObsidianInventoryItemInstance> ItemInstance = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Obsidian|Equipement")
	FGameplayTag SlotTag = FGameplayTag::EmptyTag;

	UPROPERTY(BlueprintReadOnly, Category = "Obsidian|Equipement")
	FGameplayTag SlotTagToClear = FGameplayTag::EmptyTag;

	UPROPERTY(BlueprintReadOnly, Category = "Obsidian|Equipement")
	EObsidianEquipmentChangeType ChangeType = EObsidianEquipmentChangeType::ECT_None;
};

/**
 * 
 */
USTRUCT(BlueprintType)
struct FObsidianEquipmentSlotDefinition
{
	GENERATED_BODY()

public:
	FObsidianEquipmentSlotDefinition(){}
	FObsidianEquipmentSlotDefinition(const FGameplayTag& InSlotTag, const FGameplayTagContainer& InAcceptedEquipmentCategories)
		: BaseSlotDefinition(InSlotTag, InAcceptedEquipmentCategories)
	{};
	FObsidianEquipmentSlotDefinition(const FGameplayTag& InSlotTag, const FGameplayTag& InSisterSlotTag, const FGameplayTagContainer& InAcceptedEquipmentCategories)
		: BaseSlotDefinition(InSlotTag, InAcceptedEquipmentCategories)
		, SisterSlotTag(InSisterSlotTag)
	{};

	bool IsValid() const;

	FGameplayTag GetEquipmentSlotTag() const;
	
	EObsidianPlacingAtSlotResult CanEquipAtSlot(const FGameplayTag& InItemCategory) const;

	void AddBannedEquipmentCategory(const FGameplayTag& InBannedCategory);
	void AddBannedEquipmentCategories(const FGameplayTagContainer& InBannedCategories);
	
	void RemoveBannedEquipmentCategory(const FGameplayTag& InBannedCategoryToRemove);
	void RemoveBannedEquipmentCategories(const FGameplayTagContainer& InBannedCategoriesToRemove);

public:
	FObsidianSlotDefinition BaseSlotDefinition = FObsidianSlotDefinition();
	
	/** Gameplay Tag representing a sister slot (used for weapon set) which will also be checked if the Player tries to equip specific armament (e.g. Two-Handed Weapons). */
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	FGameplayTag SisterSlotTag = FGameplayTag::EmptyTag;
	
	static const FObsidianEquipmentSlotDefinition InvalidSlot;
};

/**
 * Single item in the equipment list.
 */
USTRUCT(BlueprintType)
struct FObsidianEquipmentEntry : public FFastArraySerializerItem
{
	GENERATED_BODY();

public:
	FObsidianEquipmentEntry()
		: Instance(nullptr)
	{}
	FObsidianEquipmentEntry(UObsidianInventoryItemInstance* InInstance)
		: Instance(InInstance)
	{}
	FObsidianEquipmentEntry(UObsidianInventoryItemInstance* InInstance, const FGameplayTag& InTag)
		: Instance(InInstance)
		, EquipmentSlotTag(InTag)
	{}
	
private:
	friend FObsidianEquipmentList;
	friend UObsidianEquipmentComponent;

	UPROPERTY()
	TObjectPtr<UObsidianInventoryItemInstance> Instance = nullptr;

	UPROPERTY()
	FGameplayTag EquipmentSlotTag = FGameplayTag::EmptyTag;

	UPROPERTY()
	FGameplayTag LastObservedEquipmentSlotTag = FGameplayTag::EmptyTag;

	UPROPERTY()
	bool bSwappedOut = false;
	
	/** Authority-only list of granted handles to remove when Item unequipped. **/
	UPROPERTY(NotReplicated)
	FObsidianAffixAbilitySet_GrantedHandles GrantedHandles;
};

/**
 * Replicated equipment list that hold items equipped by the Player.
*/
USTRUCT(BlueprintType)
struct FObsidianEquipmentList : public FFastArraySerializer
{
	GENERATED_BODY();

public:
	FObsidianEquipmentList()
		: OwnerComponent(nullptr)
	{}
	FObsidianEquipmentList(UActorComponent* InOwnerComponent)
		: OwnerComponent(InOwnerComponent)
	{}

	TArray<UObsidianInventoryItemInstance*> GetAllEquippedItems() const;
	TArray<UObsidianInventoryItemInstance*> GetSwappedWeapons();
	TArray<UObsidianInventoryItemInstance*> GetEquippedWeapons();
	UObsidianInventoryItemInstance* GetEquipmentPieceByTag(const FGameplayTag& InSlotTag) const;

	UObsidianAbilitySystemComponent* GetObsidianAbilitySystemComponent() const;
	AObsidianHero* GetObsidianHero() const;
	
	FObsidianEquipmentSlotDefinition FindEquipmentSlotByTag(const FGameplayTag& InSlotTag);
	TArray<FObsidianEquipmentSlotDefinition> FindMatchingEquipmentSlotsForItemCategory(const FGameplayTag& InItemCategory);
	
	UObsidianInventoryItemInstance* AddEntry(const TSubclassOf<UObsidianInventoryItemDefinition>& InItemDefClass, const FObsidianItemGeneratedData& InItemGeneratedData,
		const FGameplayTag& InEquipmentSlotTag);
	void AddEntry(UObsidianInventoryItemInstance* InInstance, const FGameplayTag& InEquipmentSlotTag);
	UObsidianInventoryItemInstance* LoadEntry(const FObsidianSavedItem& InEquippedSavedItem);
	void RemoveEntry(UObsidianInventoryItemInstance* InInstance);
	
	void MoveWeaponToSwap(UObsidianInventoryItemInstance* InInstance);
	void MoveWeaponFromSwap(UObsidianInventoryItemInstance* InInstance);

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& InOutDeltaParams)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FObsidianEquipmentEntry, FObsidianEquipmentList>(Entries, InOutDeltaParams, *this);
	}
	
	static bool ValidateEquipmentSlot(const FGameplayTag& InSlotTag);

	//~ Start of FFastArraySerializer contract
	void PreReplicatedRemove(const TArrayView<int32> InRemovedIndices, int32 InFinalSize);
	void PostReplicatedAdd(const TArrayView<int32> InAddedIndices, int32 InFinalSize);
	void PostReplicatedChange(const TArrayView<int32> InChangedIndices, int32 InFinalSize);
	//~ End of FFastArraySerializer contract

private:
	void BroadcastChangeMessage(const FObsidianEquipmentEntry& InEntry, const FGameplayTag& InEquipmentSlotTag, const FGameplayTag& InSlotTagToClear, const EObsidianEquipmentChangeType InChangeType) const;

	void AddItemAffixesToOwner(UObsidianInventoryItemInstance* InFromItemInstance, FObsidianAffixAbilitySet_GrantedHandles* OutItemGrantedHandles);
	UObsidianAffixAbilitySet* GetDefaultAffixSet();
	
private:
	friend UObsidianEquipmentComponent;
	
	/** Replicated list of all equipment. */
	UPROPERTY()
	TArray<FObsidianEquipmentEntry> Entries;

	UPROPERTY(NotReplicated)
	TObjectPtr<UActorComponent> OwnerComponent;

	UPROPERTY()
	UObsidianAffixAbilitySet* CachedDefaultAbilitySet = nullptr;
	
	TArray<FObsidianEquipmentSlotDefinition> EquipmentSlots;
	TMap<FGameplayTag, UObsidianInventoryItemInstance*> SlotToEquipmentMap;
};

template<>
struct TStructOpsTypeTraits<FObsidianEquipmentList> : public TStructOpsTypeTraitsBase2<FObsidianEquipmentList>
{
	enum
	{
		WithNetDeltaSerializer = true
	};
};