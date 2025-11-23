// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include <CoreMinimal.h>

#include <GameFramework/SaveGame.h>

#include "ObsidianTypes/ObsidianCoreTypes.h"
#include "ObsidianSaveGame.generated.h"

class UObsidianSaveGameSubsystem;
class AObsidianHero;

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
	FText PlayerHeroName = FText();

	UPROPERTY()
	TSoftClassPtr<AObsidianHero> HeroObjectClass;

	UPROPERTY()
	EObsidianHeroClass HeroClass = EObsidianHeroClass::None;
 
	UPROPERTY()
	uint8 HeroID = INDEX_NONE;

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
};

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianSaveGame : public ULocalPlayerSaveGame
{
	GENERATED_BODY()

public:
	void InitWithSaveSystem(UObsidianSaveGameSubsystem* InSaveGameSubsystem);
	
	void InitializeHeroSaveData(const FObsidianHeroInitializationSaveData& InInitializationSaveData);
	void SetHeroGameplayData(const FObsidianHeroGameplaySaveData& InGameplaySaveData);
	
	FObsidianHeroSaveData GetHeroSaveData();
	
	virtual void HandlePostSave(bool bSuccess) override;
	virtual void HandlePostLoad() override;
		
protected:
	UPROPERTY()
	FObsidianHeroSaveData HeroSaveData;

private:
	TSoftObjectPtr<UObsidianSaveGameSubsystem> SaveGameSubsystem;
};
