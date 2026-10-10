// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"

#include "ObsidianHeroSaveGame.h"

#include "ObsidianMasterSaveGame.generated.h"

struct FObsidianHeroInitializationSaveData;

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
	FString HeroName = FString();

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
	FObsidianAddHeroSaveResult AddHero(const bool bInOnline, const FObsidianHeroInitializationSaveData& InHeroSaveData);
	bool DeleteHero(const uint16 InSaveID, const bool bInOnline);
	bool UpdateHeroSave(const uint16 InSaveID, const bool bInOnline, const uint8 InHeroLevel);
	
	TArray<FObsidianHeroSaveInfo> GetHeroSaveInfos(const bool bInOnline);
	FString GetSaveNameForID(const uint16 InSaveID, const bool bInOnline) const; 
	uint16 GetMaxOfflineSaveID() const; 
	uint16 GetMaxOnlineSaveID() const; 

protected:
	/** Adds new Offline Hero, returns new generated Save name as well as its ID. */
	FObsidianAddHeroSaveResult AddOfflineHero(const FObsidianHeroInitializationSaveData& InHeroSaveData);
	/** Adds new Online Hero, returns new generated Save name as well as its ID. */
	FObsidianAddHeroSaveResult AddOnlineHero(const FObsidianHeroInitializationSaveData& InHeroSaveData);

	FObsidianHeroSaveInfo* GetHeroSaveInfo(const uint16 InSaveID, const bool bInOnline);
	
protected:
	UPROPERTY()
	FObsidianMasterSaveParams MasterSaveParams = FObsidianMasterSaveParams();
};
