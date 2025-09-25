// Copyright 2024 out of sCope team - Michał Ogiński

#include "InventoryItems/ItemDrop/ObsidianItemDataLoaderSubsystem.h"

// ~ Core
#include "Engine/AssetManager.h"

// ~ Project
#include "InventoryItems/ItemDrop/ObsidianItemDataConfig.h"
#include "InventoryItems/ItemDrop/ObsidianItemDataDeveloperSettings.h"

DEFINE_LOG_CATEGORY(LogItemDataLoader);

void UObsidianItemDataLoaderSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	LoadItemDataConfig();
}

void UObsidianItemDataLoaderSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

TArray<FObsidianTreasureClass> UObsidianItemDataLoaderSubsystem::GetAllTreasureClassesUpToQuality(const int32 TreasureQuality) const
{
	TArray<FObsidianTreasureClass> GatheredClasses;
	if (ItemDataConfig == nullptr)
	{
		return GatheredClasses;
	}
	
	for (TSoftObjectPtr<UObsidianTreasureList> TreasureList : ItemDataConfig->CommonTreasureLists)
	{
		if (UObsidianTreasureList* LoadedList = TreasureList.Get())
		{
			GatheredClasses.Append(LoadedList->GetAllTreasureClassesUpToQuality(TreasureQuality));
		}
		else if (UObsidianTreasureList* SynchronousLoadedList = TreasureList.LoadSynchronous())
		{
			UE_LOG(LogItemDataLoader, Warning, TEXT("Treasure List was not loaded correctly, falling back to LoadSynchronous() instead."))
			GatheredClasses.Append(SynchronousLoadedList->GetAllTreasureClassesUpToQuality(TreasureQuality));
		}
	}
	
	return GatheredClasses;
}

void UObsidianItemDataLoaderSubsystem::LoadItemDataConfig()
{
	const UObsidianItemDataDeveloperSettings* ItemDataSettings = GetDefault<UObsidianItemDataDeveloperSettings>();
	if (ItemDataSettings == nullptr)
	{
		UE_LOG(LogItemDataLoader, Error, TEXT("ObsidianTreasureConfigDeveloperSettings was not found! Abandoning Loading Item Data Config."));
		return;
	}

	
	const TSoftObjectPtr<UObsidianItemDataConfig>& ItemDataConfigRef = ItemDataSettings->ItemDataConfig;
	if (ItemDataConfigRef.IsNull() == false)
	{
		FSoftObjectPath ConfigPathToLoad = ItemDataConfigRef.ToSoftObjectPath();

		UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(
				ConfigPathToLoad,
				FStreamableDelegate::CreateUObject(this, &ThisClass::OnItemDataLoaded)
			);
	}
}

void UObsidianItemDataLoaderSubsystem::OnItemDataLoaded()
{
	if (const UObsidianItemDataDeveloperSettings* ItemDataSettings = GetDefault<UObsidianItemDataDeveloperSettings>())
	{
		if (ItemDataSettings->ItemDataConfig)
		{
			ItemDataConfig = ItemDataSettings->ItemDataConfig.Get();
			UE_LOG(LogItemDataLoader, Log, TEXT("Loaded Treasure Config: [%s]."), *ItemDataConfig->GetName());
		}
	}
}
