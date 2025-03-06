// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "ObsidianEquipmentList.generated.h"

class UObsidianInventoryItemInstance;
class UObsidianInventoryItemDefinition;
class UObsidianEquipmentComponent;
struct FObsidianEquipmentList;

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
	FGameplayTag EquipmentSlotTag;
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

	UObsidianInventoryItemInstance* AddEntry(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDefClass, const FVector2D& EquipmentSlotTag);
	void AddEntry(UObsidianInventoryItemInstance* Instance, const FGameplayTag& EquipmentSlotTag);
	void RemoveEntry(UObsidianInventoryItemInstance* Instance);

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FObsidianEquipmentEntry, FObsidianEquipmentList>(Entries, DeltaParams, *this);
	}

	//~ Start of FFastArraySerializer contract
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
	//~ End of FFastArraySerializer contract

private:
	friend UObsidianEquipmentComponent;
	
	/** Replicated list of all equipment. */
	UPROPERTY()
	TArray<FObsidianEquipmentEntry> Entries;

	UPROPERTY(NotReplicated)
	TObjectPtr<UActorComponent> OwnerComponent;
	
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