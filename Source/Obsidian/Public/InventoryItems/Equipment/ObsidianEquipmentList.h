// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

// ~ Project
#include "AbilitySystem/Data/ObsidianAbilitySet.h"
#include "ObsidianTypes/ItemTypes/ObsidianItemTypes.h"

#include "Net/Serialization/FastArraySerializer.h"
#include "ObsidianEquipmentList.generated.h"

class UObsidianAbilitySystemComponent;
class UObsidianInventoryItemInstance;
class UObsidianInventoryItemDefinition;
class UObsidianEquipmentComponent;
struct FObsidianEquipmentList;

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
	
	EObsidianPlacingAtSlotResult CanEquipAtSlot(const FGameplayTag& ItemCategory) const;

	void AddBannedEquipmentCategory(const FGameplayTag& InBannedCategory);
	void AddBannedEquipmentCategories(const FGameplayTagContainer& InBannedCategories);
	
	void RemoveBannedEquipmentCategory(const FGameplayTag& BannedCategoryToRemove);
	void RemoveBannedEquipmentCategories(const FGameplayTagContainer& BannedCategoriesToRemove);

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
	FObsidianAbilitySet_GrantedHandles GrantedHandles;
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
	UObsidianInventoryItemInstance* GetEquipmentPieceByTag(const FGameplayTag& SlotTag) const;

	UObsidianAbilitySystemComponent* GetObsidianAbilitySystemComponent() const;
	
	FObsidianEquipmentSlotDefinition FindEquipmentSlotByTag(const FGameplayTag& SlotTag);
	TArray<FObsidianEquipmentSlotDefinition> FindMatchingEquipmentSlotsForItemCategory(const FGameplayTag& ItemCategory);
	
	UObsidianInventoryItemInstance* AddEntry(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDefClass, const FGameplayTag& EquipmentSlotTag);
	void AddEntry(UObsidianInventoryItemInstance* Instance, const FGameplayTag& EquipmentSlotTag);
	void RemoveEntry(UObsidianInventoryItemInstance* Instance);
	
	void MoveWeaponToSwap(UObsidianInventoryItemInstance* Instance);
	void MoveWeaponFromSwap(UObsidianInventoryItemInstance* Instance);

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FObsidianEquipmentEntry, FObsidianEquipmentList>(Entries, DeltaParams, *this);
	}
	
	static bool ValidateEquipmentSlot(const FGameplayTag& SlotTag);

	//~ Start of FFastArraySerializer contract
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
	//~ End of FFastArraySerializer contract

private:
	void BroadcastChangeMessage(const FObsidianEquipmentEntry& Entry, const FGameplayTag& EquipmentSlotTag, const FGameplayTag& SlotTagToClear, const EObsidianEquipmentChangeType ChangeType) const;

private:
	friend UObsidianEquipmentComponent;
	
	/** Replicated list of all equipment. */
	UPROPERTY()
	TArray<FObsidianEquipmentEntry> Entries;

	UPROPERTY(NotReplicated)
	TObjectPtr<UActorComponent> OwnerComponent;
	
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