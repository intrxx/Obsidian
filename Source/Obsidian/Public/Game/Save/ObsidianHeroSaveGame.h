// Copyright 2026 out of sCope team - intrxx

#pragma once

#include <CoreMinimal.h>

#include "ObsidianTypes/ObsidianCoreTypes.h"
#include "ObsidianTypes/ItemTypes/ObsidianItemTypes.h"
#include "ObsidianTypes/ObsidianSavedTypes.h"

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
	bool bReceivedInitialInventoryItems = false;

	UPROPERTY()
	TArray<FObsidianSavedItem> EquipmentSavedItems;

	UPROPERTY()
	bool bReceivedInitialEquipmentItems = false;

	UPROPERTY()
	TArray<FObsidianSavedItem> PersonalStashedItems;
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
