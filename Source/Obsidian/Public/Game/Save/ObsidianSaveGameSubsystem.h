// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include <CoreMinimal.h>

#include <Subsystems/GameInstanceSubsystem.h>
#include "ObsidianSaveGameSubsystem.generated.h"

struct FObsidianHeroInitializationSaveData;
class AObsidianPlayerController;
class UObsidianSaveGame;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnSaveActionFinishedSignature, UObsidianSaveGame* SaveObject)

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianSaveGameSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UObsidianSaveGame* GetSaveGameObject();
	
	void CreateSaveGameObject();
	
	void RegisterSaveable(AActor* SaveActor);
	void UnregisterSaveable(AActor* SaveActor);
	
	void RequestSaveGame(const bool bAsync);
	void RequestSaveInitialHeroSave(const bool bAsync, const FObsidianHeroInitializationSaveData& HeroInitializationSaveData);
	
	void RequestLoadGame(const bool bAsync);
	void RequestLoadDataForObject(AActor* LoadActor);
	
public:
	FOnSaveActionFinishedSignature OnSavingFinishedDelegate;
	FOnSaveActionFinishedSignature OnLoadingFinishedDelegate;

protected:
	void SaveGameForPlayer();
	void SaveGameForPlayerAsync();
	
	void LoadGameForPlayer();
	void LoadGameForPlayerAsync();
	
protected:
	UPROPERTY()
	TArray<TWeakObjectPtr<AActor>> SaveableActors;
	
	UPROPERTY()
	TObjectPtr<UObsidianSaveGame> ObsidianSaveGame;

	UPROPERTY()
	FString SaveSlotName = FString();
};
