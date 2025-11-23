// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include <CoreMinimal.h>

#include <Subsystems/GameInstanceSubsystem.h>
#include "ObsidianSaveGameSubsystem.generated.h"

class UObsidianLocalPlayer;
struct FObsidianHeroInitializationSaveData;
class AObsidianPlayerController;
class UObsidianSaveGame;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnSaveActionFinishedSignature, UObsidianSaveGame* SaveObject, bool bSuccess)

DECLARE_LOG_CATEGORY_EXTERN(LogObsidianSaveSystem, Log, All)

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianSaveGameSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UObsidianSaveGame* GetSaveGameObject();
	
	void CreateSaveGameObject(const UObsidianLocalPlayer* LocalPlayer);
	
	void RegisterSaveable(AActor* SaveActor);
	void UnregisterSaveable(AActor* SaveActor);
	
	void RequestSaveGame(const bool bAsync, const UObsidianLocalPlayer* LocalPlayer);
	void RequestSaveInitialHeroSave(const bool bAsync, const FObsidianHeroInitializationSaveData& HeroInitializationSaveData ,
		const UObsidianLocalPlayer* LocalPlayer);
	
	void RequestLoadGame(const bool bAsync, const UObsidianLocalPlayer* LocalPlayer);
	void RequestLoadDataForObject(AActor* LoadActor);
	
public:
	FOnSaveActionFinishedSignature OnSavingFinishedDelegate;
	FOnSaveActionFinishedSignature OnLoadingFinishedDelegate;

protected:
	void SaveGameForPlayer(const UObsidianLocalPlayer* LocalPlayer);
	void SaveGameForPlayerAsync(const UObsidianLocalPlayer* LocalPlayer);
	
	void LoadGameForPlayer(const UObsidianLocalPlayer* LocalPlayer);
	void LoadGameForPlayerAsync(const UObsidianLocalPlayer* LocalPlayer);
	
protected:
	UPROPERTY()
	TArray<TWeakObjectPtr<AActor>> SaveableActors;
	
	UPROPERTY()
	TObjectPtr<UObsidianSaveGame> ObsidianSaveGame;

	UPROPERTY()
	FString SaveSlotName = FString();

private:
	void HandleLoadingFinished(UObsidianSaveGame* SaveGame);
	void HandleSavingFinished(const bool bSuccess, UObsidianSaveGame* SaveGame);

private:
	friend UObsidianSaveGame;
};
