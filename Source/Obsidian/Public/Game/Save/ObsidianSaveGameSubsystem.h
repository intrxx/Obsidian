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
	
	void RegisterSaveable(AActor* SaveActor);
	void UnregisterSaveable(AActor* SaveActor);

	void LoadOrCreateMasterSaveObject(const UObsidianLocalPlayer* LocalPlayer);
	
	void RequestSaveGame(const UObsidianLocalPlayer* LocalPlayer, const bool bAsync);
	void RequestSaveInitialHeroSave(const UObsidianLocalPlayer* LocalPlayer, const bool bAsync, const bool bOnline,
		const FObsidianHeroInitializationSaveData& HeroInitializationSaveData);
	
	void RequestLoadHeroSaveGameWithID(const UObsidianLocalPlayer* LocalPlayer, const bool bAsync, const uint16 SaveID,
		const bool bOnline);
	void RequestLoadGame(const UObsidianLocalPlayer* LocalPlayer, const bool bAsync, const FString& SlotName);
	void RequestLoadDataForObject(AActor* LoadActor);

	bool DeleteHeroSave(const uint16 SaveID, const bool bOnline);
	
public:
	FOnSaveActionFinishedSignature OnSavingFinishedDelegate;
	FOnSaveActionFinishedSignature OnLoadingFinishedDelegate;

protected:
	void SaveHeroGameForPlayer();
	void SaveHeroGameForPlayerAsync();
	
	void LoadGameForPlayer(const UObsidianLocalPlayer* LocalPlayer, const FString& SlotName);
	void LoadGameForPlayerAsync(const UObsidianLocalPlayer* LocalPlayer, const FString& SlotName);

	UObsidianHeroSaveGame* CreateHeroSaveGameObject(const UObsidianLocalPlayer* LocalPlayer, const FString& SlotName,
		const uint16 SaveID);
	
protected:
	UPROPERTY()
	TArray<TWeakObjectPtr<AActor>> SaveableActors;
	
	UPROPERTY()
	TObjectPtr<UObsidianHeroSaveGame> CurrentHeroSaveGame;

	UPROPERTY()
	TObjectPtr<UObsidianMasterSaveGame> ObsidianMasterSaveGame;

private:
	void HandleLoadingHeroSaveFinished(UObsidianHeroSaveGame* SaveGame);
	void HandleSavingHeroSaveFinished(const bool bSuccess, UObsidianHeroSaveGame* SaveGame);

private:
	friend UObsidianHeroSaveGame;
};
