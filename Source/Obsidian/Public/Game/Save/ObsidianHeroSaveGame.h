// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include <CoreMinimal.h>


#include "InventoryItems/Fragments/OInventoryItemFragment_Usable.h"
#include "InventoryItems/Fragments/OInventoryItemFragment_Equippable.h"
#include "ObsidianTypes/ObsidianCoreTypes.h"
#include "ObsidianTypes/ItemTypes/ObsidianItemTypes.h"

#include <GameFramework/SaveGame.h>
#include "ObsidianHeroSaveGame.generated.h"

struct FObsidianEquipmentActor;

class UObsidianSaveGameSubsystem;
class AObsidianHero;
class AObsidianSpawnedEquipmentPiece;

USTRUCT()
struct FObsidianSavedItemRequirements
{
	GENERATED_BODY()

public:
	EObsidianHeroClass HeroClassRequirement = EObsidianHeroClass::None;

	UPROPERTY()
	TArray<FObsidianAttributeRequirement> AttributeRequirements;

	UPROPERTY()
	uint8 RequiredLevel = 0;

	UPROPERTY()
	uint8 bHasAnyRequirements:1;

	UPROPERTY()
	uint8 bInitialized:1;
};

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

	//TODO(intrxx) Do actual affixes here. 

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

/**
 * Hero Attributes that can be saved in generic way like Experience.
 * They must not depend on any other attribute.
 */
USTRUCT()
struct FObsidianGenericAttributes
{
	GENERATED_BODY()

public:
	UPROPERTY()
	float CurrentExperience = 0.0f;
	
	UPROPERTY()
	float MaxExperience = 0.0f;

	UPROPERTY()
	float PassiveSkillPoints = 0.0f;

	UPROPERTY()
	float AscensionPoints = 0.0f;
};

/**
 * Hero SaveData that will update during Gameplay.
 */
USTRUCT()
struct FObsidianHeroGameplaySaveData
{
	GENERATED_BODY()

public:
	UPROPERTY()
	uint8 HeroLevel = 1;
	
	UPROPERTY()
	FVector CurrentLocation = FVector::ZeroVector;

	UPROPERTY()
	FObsidianGenericAttributes GenericStatAttributes;

	UPROPERTY()
	TArray<FObsidianSavedItem> InventorySavedItems;

	UPROPERTY()
	TArray<FObsidianSavedItem> EquipmentSavedItems;
};

/**
 * The SaveData that will be initialized only once, like character name or class.
 */
USTRUCT()
struct FObsidianHeroInitializationSaveData
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FString PlayerHeroName = FString();

	UPROPERTY()
	TSoftClassPtr<AObsidianHero> HeroObjectClass;

	UPROPERTY()
	EObsidianHeroClass HeroClass = EObsidianHeroClass::None;
	
	UPROPERTY()
	uint8 bHardcore:1 = false;
};

/**
 * 
 */
USTRUCT()
struct FObsidianHeroSaveData
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FObsidianHeroInitializationSaveData InitializationSaveData;

	UPROPERTY()
	FObsidianHeroGameplaySaveData GameplaySaveData;

	UPROPERTY()
	uint16 SaveID = INDEX_NONE;

	UPROPERTY()
	bool bOnline = false;
};

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianHeroSaveGame : public ULocalPlayerSaveGame
{
	GENERATED_BODY()

public:
	void InitWithSaveSystem(UObsidianSaveGameSubsystem* InSaveGameSubsystem);
	
	void InitializeHeroSaveData(const bool InbOnline, const FObsidianHeroInitializationSaveData& InInitializationSaveData);
	
	void SetHeroGameplayData(const FObsidianHeroGameplaySaveData& InGameplaySaveData);
	
	FObsidianHeroSaveData GetHeroSaveData();

	bool IsOnline() const;
	void SetSaveID(const uint16 InSaveID);
	uint16 GetSaveID() const;
	uint8 GetHeroLevel() const;
	
	virtual void HandlePostSave(bool bSuccess) override;
	virtual void HandlePostLoad() override;
		
protected:
	UPROPERTY()
	FObsidianHeroSaveData HeroSaveData;

private:
	TSoftObjectPtr<UObsidianSaveGameSubsystem> SaveGameSubsystem;
};
