// Copyright 2024 out of sCope team - Michał Ogiński

#include "InventoryItems/ItemDrop/ObsidianItemDataLoaderSubsystem.h"

// ~ Core
#include "Engine/AssetManager.h"

// ~ Project
#include "InventoryItems/ItemDrop/ObsidianTreasureConfig.h"
#include "InventoryItems/ItemDrop/ObsidianItemDataDeveloperSettings.h"

void UObsidianItemDataLoaderSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	LoadTreasureConfig();
}

void UObsidianItemDataLoaderSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

TArray<FObsidianTreasureClass> UObsidianItemDataLoaderSubsystem::GetAllTreasureClassesUpToQuality(const int32 TreasureQuality) const
{
	TArray<FObsidianTreasureClass> GatheredClasses;
	if (TreasureConfig == nullptr)
	{
		return GatheredClasses;
	}
	
	for (TSoftObjectPtr<UObsidianTreasureList> TreasureList : TreasureConfig->CommonTreasureLists)
	{
		if (UObsidianTreasureList* LoadedList = TreasureList.LoadSynchronous())
		{
			GatheredClasses.Append(LoadedList->GetAllTreasureClassesUpToQuality(TreasureQuality));
		}
	}
	
	return GatheredClasses;
}

void UObsidianItemDataLoaderSubsystem::LoadTreasureConfig()
{
	const UObsidianItemDataDeveloperSettings* Settings = GetDefault<UObsidianItemDataDeveloperSettings>();
	if (Settings == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("ObsidianTreasureConfigDeveloperSettings not found!"));
		return;
	}

	const TSoftObjectPtr<UObsidianTreasureConfig>& ConfigRef = Settings->TreasureConfig;
	if (ConfigRef.IsNull() == false)
	{
		FSoftObjectPath TreasureConfigPath = ConfigRef.ToSoftObjectPath();
		
		UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(
				TreasureConfigPath,
				FStreamableDelegate::CreateUObject(this, &ThisClass::OnTreasureConfigLoaded)
			);
	}
}

void UObsidianItemDataLoaderSubsystem::OnTreasureConfigLoaded()
{
	const UObsidianItemDataDeveloperSettings* Settings = GetDefault<UObsidianItemDataDeveloperSettings>();
	if (Settings && Settings->TreasureConfig)
	{
		TreasureConfig = Settings->TreasureConfig.Get();
		UE_LOG(LogTemp, Log, TEXT("Loaded Treasure Config: %s"), *TreasureConfig->GetName());
	}
}
