// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include <CoreMinimal.h>


#include <GameFramework/SaveGame.h>

#include "ObsidianHeroSaveGame.h"
#include "ObsidianMasterSaveGame.generated.h"

struct FObsidianHeroInitializationSaveData;

namespace ObsidianMasterSaveStatics
{
	const inline FString MasterSaveName = FString("ObsidianSaveGame");
}

USTRUCT()
struct FObsidianAddHeroSaveResult
{
	GENERATED_BODY()

public:
	FObsidianAddHeroSaveResult(){}
	FObsidianAddHeroSaveResult(const FString& InSaveName, const uint16 InSaveID)
		: SaveName(InSaveName)
		, SaveID(InSaveID)
	{}
	
public:
	UPROPERTY()
	FString SaveName = FString();

	UPROPERTY()
	uint16 SaveID = INDEX_NONE;
};

/**
 *
 */
USTRUCT()
struct FObsidianHeroDescription
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FText HeroName = FText();

	UPROPERTY()
	EObsidianHeroClass HeroClass = EObsidianHeroClass::None;

	UPROPERTY()
	uint8 HeroLevel = INDEX_NONE;

	UPROPERTY()
	uint8 bHardcore:1 = false;
};

/**
 * 
 */
USTRUCT()
struct FObsidianHeroSaveInfo
{
	GENERATED_BODY()

public:
	bool IsOnline() const;
	
public:
	UPROPERTY()
	uint16 SaveID = INDEX_NONE;
	
	UPROPERTY()
	FString SaveName = FString();

	UPROPERTY()
	uint8 bOnline:1 = false;
	
	UPROPERTY()
	FObsidianHeroDescription HeroDescription = FObsidianHeroDescription();
};

/**
 * 
 */
USTRUCT()
struct FObsidianMasterSaveParams
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TArray<FObsidianHeroSaveInfo> OfflineSavedHeroes;

	UPROPERTY()
	TArray<FObsidianHeroSaveInfo> OnlineSavedHeroes;
};

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianMasterSaveGame : public ULocalPlayerSaveGame
{
	GENERATED_BODY()

public:
	FObsidianAddHeroSaveResult AddHero(const bool bOnline, const FObsidianHeroInitializationSaveData& HeroSaveData);
	bool DeleteHero(const uint16 SaveID, const bool bOnline);
	bool UpdateHeroSave(const uint16 SaveID, const bool bOnline, const uint8 HeroLevel);
	
	TArray<FObsidianHeroSaveInfo> GetHeroSaveInfos(const bool bOnline);
	FString GetSaveNameForID(const uint16 SaveID, const bool bOnline) const; 
	uint16 GetMaxOfflineSaveID() const; 
	uint16 GetMaxOnlineSaveID() const; 

protected:
	/** Adds new Offline Hero, returns new generated Save name as well as its ID. */
	FObsidianAddHeroSaveResult AddOfflineHero(const FObsidianHeroInitializationSaveData& HeroSaveData);
	/** Adds new Online Hero, returns new generated Save name as well as its ID. */
	FObsidianAddHeroSaveResult AddOnlineHero(const FObsidianHeroInitializationSaveData& HeroSaveData);

	FObsidianHeroSaveInfo* GetHeroSaveInfo(const uint16 SaveID, const bool bOnline);
	
protected:
	UPROPERTY()
	FObsidianMasterSaveParams MasterSaveParams = FObsidianMasterSaveParams();
};
