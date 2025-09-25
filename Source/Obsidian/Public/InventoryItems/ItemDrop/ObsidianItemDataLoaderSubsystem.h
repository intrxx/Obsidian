// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project
#include "ObsidianTreasureList.h"

#include "Subsystems/GameInstanceSubsystem.h"
#include "ObsidianItemDataLoaderSubsystem.generated.h"

class UObsidianItemDataDeveloperSettings;
class UObsidianItemDataConfig;
class UObsidianTreasureList;

DECLARE_LOG_CATEGORY_EXTERN(LogItemDataLoader, Log, All);

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianItemDataLoaderSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
	TArray<FObsidianTreasureClass> GetAllTreasureClassesUpToQuality(const int32 TreasureQuality) const;
	
protected:
	void LoadItemDataConfig();
	void OnItemDataLoaded();
	
protected:
	UPROPERTY()
	TObjectPtr<UObsidianItemDataConfig> ItemDataConfig;
};
