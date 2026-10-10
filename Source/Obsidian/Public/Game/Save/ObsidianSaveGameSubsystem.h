// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "ObsidianTypes/ObsidianCoreTypes.h"

#include "ObsidianSaveGameSubsystem.generated.h"

struct FObsidianHeroSaveInfo;
struct FObsidianHeroInitializationSaveData;

class UObsidianSharedStashSaveGame;
class UObsidianLocalPlayer;
class AObsidianPlayerController;
class UObsidianHeroSaveGame;
class UObsidianMasterSaveGame;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnSaveActionFinishedSignature, UObsidianHeroSaveGame* SaveObject, bool bSuccess)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnSharedStashDataLoadedSignature, UObsidianSharedStashSaveGame* StashSaveObject)

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianSaveGameSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UObsidianHeroSaveGame* GetCurrentHeroSaveGameObject();
	UObsidianSharedStashSaveGame* GetStashSaveGameObject(const EObsidianGameNetworkType InNetworkType);

	bool FillSaveInfosFromMasterSave(const bool bInOnline, const UObsidianLocalPlayer* InLocalPlayer,
		TArray<FObsidianHeroSaveInfo>& OutHeroInfos);
	
	void RegisterSaveable(AActor* InSaveActor);
	void UnregisterSaveable(AActor* InSaveActor);

	void LoadOrCreateMasterSaveObject(const UObsidianLocalPlayer* InLocalPlayer);
	void AsyncLoadOrCreateSharedStashDataSaveObject(const UObsidianLocalPlayer* InLocalPlayer, const bool bInOnline);
	
	void RequestSaveGame(const UObsidianLocalPlayer* InLocalPlayer, const bool bInAsync);
	void RequestSaveInitialHeroSave(const UObsidianLocalPlayer* InLocalPlayer, const bool bInAsync, const bool bInOnline,
		const FObsidianHeroInitializationSaveData& InHeroInitializationSaveData);
	void AsyncSaveSharedStashData(const AObsidianPlayerController* InPlayerController, const EObsidianGameNetworkType InNetworkType);
	
	void RequestLoadHeroSaveGameWithID(const UObsidianLocalPlayer* InLocalPlayer, const bool bInAsync, const uint16 InSaveID,
		const bool bInOnline);
	void RequestLoadGame(const UObsidianLocalPlayer* InLocalPlayer, const bool bInAsync, const FString& InSlotName);
	void RequestLoadDataForObject(AActor* InLoadActor);

	bool DeleteHeroSave(const uint16 InSaveID, const bool bInOnline);
	
public:
	FOnSaveActionFinishedSignature OnSavingFinishedDelegate;
	FOnSaveActionFinishedSignature OnLoadingFinishedDelegate;
	
	FOnSharedStashDataLoadedSignature OnSharedStashDataLoadedDelegate;

protected:
	void SaveHeroGameForPlayer();
	void SaveHeroGameForPlayerAsync();
	
	void LoadGameForPlayer(const UObsidianLocalPlayer* InLocalPlayer, const FString& InSlotName);
	void LoadGameForPlayerAsync(const UObsidianLocalPlayer* InLocalPlayer, const FString& InSlotName);

	UObsidianHeroSaveGame* CreateHeroSaveGameObject(const UObsidianLocalPlayer* InLocalPlayer, const FString& InSlotName,
		const uint16 InSaveID);
	
protected:
	UPROPERTY()
	TArray<TWeakObjectPtr<AActor>> SaveableActors;
	
	UPROPERTY()
	TObjectPtr<UObsidianHeroSaveGame> CurrentHeroSaveGame;

	UPROPERTY()
	TObjectPtr<UObsidianSharedStashSaveGame> OfflineSharedStashData;

	UPROPERTY()
	TObjectPtr<UObsidianSharedStashSaveGame> OnlineSharedStashData;

	UPROPERTY()
	TObjectPtr<UObsidianMasterSaveGame> ObsidianMasterSaveGame;

private:
	void HandleLoadingHeroSaveFinished(UObsidianHeroSaveGame* InSaveGame);
	void HandleSavingHeroSaveFinished(const bool bInSuccess, UObsidianHeroSaveGame* InSaveGame);

private:
	friend UObsidianHeroSaveGame;
};
