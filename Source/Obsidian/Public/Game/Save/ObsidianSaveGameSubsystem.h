// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include <CoreMinimal.h>

#include <Subsystems/GameInstanceSubsystem.h>
#include "ObsidianSaveGameSubsystem.generated.h"

struct FObsidianHeroSaveInfo;
struct FObsidianHeroInitializationSaveData;

class UObsidianLocalPlayer;
class AObsidianPlayerController;
class UObsidianHeroSaveGame;
class UObsidianMasterSaveGame;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnSaveActionFinishedSignature, UObsidianHeroSaveGame* SaveObject, bool bSuccess)

DECLARE_LOG_CATEGORY_EXTERN(LogObsidianSaveSystem, Log, All)

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianSaveGameSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UObsidianHeroSaveGame* GetCurrentHeroSaveGameObject();

	bool FillSaveInfosFromMasterSave(const bool bOnline, const UObsidianLocalPlayer* LocalPlayer,
		TArray<FObsidianHeroSaveInfo>& OutHeroInfos);
	
	//TODO(intrxx) Currently called before pushing main menu in GameMode, call in some better place
	void LoadOrCreateMasterSaveObject(const UObsidianLocalPlayer* LocalPlayer);
	
	void RegisterSaveable(AActor* SaveActor);
	void UnregisterSaveable(AActor* SaveActor);
	
	void RequestSaveGame(const bool bAsync, const UObsidianLocalPlayer* LocalPlayer);
	void RequestSaveInitialHeroSave(const bool bAsync, const bool bOnline,
		const FObsidianHeroInitializationSaveData& HeroInitializationSaveData,
		const UObsidianLocalPlayer* LocalPlayer);
	
	void RequestLoadGame(const bool bAsync, const UObsidianLocalPlayer* LocalPlayer);
	void RequestLoadDataForObject(AActor* LoadActor);
	
public:
	FOnSaveActionFinishedSignature OnSavingFinishedDelegate;
	FOnSaveActionFinishedSignature OnLoadingFinishedDelegate;

protected:
	void SaveHeroGameForPlayer();
	void SaveHeroGameForPlayerAsync();
	
	void LoadGameForPlayer(const UObsidianLocalPlayer* LocalPlayer);
	void LoadGameForPlayerAsync(const UObsidianLocalPlayer* LocalPlayer);

	UObsidianHeroSaveGame* CreateHeroSaveGameObject(const UObsidianLocalPlayer* LocalPlayer, const FString& SlotName);
	
protected:
	UPROPERTY()
	TArray<TWeakObjectPtr<AActor>> SaveableActors;
	
	UPROPERTY()
	TArray<TObjectPtr<UObsidianHeroSaveGame>> ObsidianHeroSaveGames;

	UPROPERTY()
	TObjectPtr<UObsidianHeroSaveGame> CurrentHeroSaveGame;

	UPROPERTY()
	TObjectPtr<UObsidianMasterSaveGame> ObsidianMasterSaveGame;

private:
	void HandleLoadingFinished(UObsidianHeroSaveGame* SaveGame);
	void HandleSavingFinished(const bool bSuccess, UObsidianHeroSaveGame* SaveGame);

private:
	friend UObsidianHeroSaveGame;
};
