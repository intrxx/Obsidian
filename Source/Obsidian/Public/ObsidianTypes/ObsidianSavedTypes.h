// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include <CoreMinimal.h>
#include <GameplayTagContainer.h>

#include "InventoryItems/Fragments/OInventoryItemFragment_Equippable.h"
#include "InventoryItems/Fragments/OInventoryItemFragment_Usable.h"

#include "ObsidianSavedTypes.generated.h"

class UObsidianInventoryItemDefinition;
class UObsidianUsableShard;

namespace ObsidianSaveStatics
{
	const inline FString MasterSaveName = FString("ObsidianSaveGame");
	const inline FString OnlineStashDataSaveName = FString("ObsidianOnlineStashDataSaveGame");
	const inline FString OfflineStashDataSaveName = FString("ObsidianOfflineStashDataSaveGame");
}

USTRUCT()
struct FObsidianSavedEquipmentPiece
{
	GENERATED_BODY()

public:
	FObsidianSavedEquipmentPiece(){}
	FObsidianSavedEquipmentPiece(const FObsidianEquipmentActor& EquipmentActor);
	
public:
	UPROPERTY()
	TSoftClassPtr<AObsidianSpawnedEquipmentPiece> SoftActorToSpawn = nullptr;
	
	UPROPERTY()
	bool bOverrideAttachSocket = true;

	UPROPERTY()
	FString AttachSocketName;

	UPROPERTY()
	FTransform AttachTransform;
};

/**
 * 
 */
USTRUCT()
struct FObsidianSavedItem
{
	GENERATED_BODY()

public:
	/**
	 * General 
	 */
	
	UPROPERTY()
	FString UniqueItemID = FString();

	UPROPERTY()
	uint8 ItemLevel = INDEX_NONE;
	
	UPROPERTY()
	TSoftClassPtr<UObsidianInventoryItemDefinition> SoftItemDef;

	UPROPERTY()
	FGameplayTag ItemCategory = FGameplayTag::EmptyTag;

	UPROPERTY()
	FGameplayTag ItemBaseType = FGameplayTag::EmptyTag;

	UPROPERTY()
	EObsidianItemRarity ItemRarity = EObsidianItemRarity::None;
	
	UPROPERTY()
	FObsidianItemPosition ItemCurrentPosition = FObsidianItemPosition();

	/**
	 * Usability
	 */

	UPROPERTY()
	bool bUsable = false;
	
	UPROPERTY()
	EObsidianUsableItemType UsableItemType = EObsidianUsableItemType::UIT_None;

	UPROPERTY()
	TSoftObjectPtr<UObsidianUsableShard> UsableShard = nullptr;

	/**
	 * Equipping.
	 */

	UPROPERTY()
	bool bEquippable = false;
	
	UPROPERTY()
	bool bNeedsTwoSlots = false;

	UPROPERTY()
	TArray<FObsidianSavedEquipmentPiece> SavedEquipmentPieces;
	
	UPROPERTY()
	FObsidianItemRequirements EquippingRequirements = FObsidianItemRequirements();

	/**
	 * Affixes.
	 */

	UPROPERTY()
	bool bIdentified = false;

	UPROPERTY()
	TArray<FObsidianActiveItemAffix> SavedAffixes;

	/**
	 * Stacks.
	 */

	UPROPERTY()
	bool bStackable = false;
	
	UPROPERTY()
	int32 ItemCurrentStacks = 0;

	UPROPERTY()
	int32 ItemMaxStacks = 0;

	UPROPERTY()
	int32 ItemLimitStacks = 0;

	/**
	 * Appearance.
	 */

	UPROPERTY()
	FIntPoint ItemGridSpan = FIntPoint::NoneValue;

	UPROPERTY()
	TSoftObjectPtr<UTexture2D> SoftItemImage = nullptr;

	UPROPERTY()
	TSoftObjectPtr<UStaticMesh> SoftItemDroppedMesh = nullptr;

	UPROPERTY()
	FString ItemDisplayName = FString();

	// ~ Start of ItemNameAdditionsData
	UPROPERTY()
	FString ItemNameAdditionsData_RareItemDisplayNameAddition = FString();

	UPROPERTY()
	FString ItemNameAdditionsData_MagicItemDisplayNameAddition = FString();
	// ~ End of ItemNameAdditionsData

	UPROPERTY()
	FString ItemDescription = FString();

	UPROPERTY()
	FString ItemAdditionalDescription = FString();
	
	UPROPERTY()
	float ItemSlotPadding = 0.0f;

	/**
	 * Debug
	 */

	UPROPERTY()
	FString DebugItemName = FString();
};