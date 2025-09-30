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
				FStreamableDelegate::CreateUObject(this, &ThisClass::OnItemConfigLoaded)
			);
	}
}

void UObsidianItemDataLoaderSubsystem::OnItemConfigLoaded()
{
	if (const UObsidianItemDataDeveloperSettings* ItemDataSettings = GetDefault<UObsidianItemDataDeveloperSettings>())
	{
		if (ItemDataSettings->ItemDataConfig)
		{
			ItemDataConfig = ItemDataSettings->ItemDataConfig.Get();
			UE_LOG(LogItemDataLoader, Log, TEXT("Loaded Treasure Config: [%s]."), *ItemDataConfig->GetName());
		}
	}
	
	TArray<FObsidianTreasureClass> TreasureClasses;
	for (const UObsidianTreasureList* TreasureList : ItemDataConfig->CommonTreasureLists)
	{
		if (TreasureList)
		{
			TreasureClasses.Append(TreasureList->GetAllTreasureClasses());
		}
	}

	//This is kind of pre-optimization stuff, but I expect this to get big in the future.
	//TODO(intrxx) Recheck the performance of this compared to regular fors on Game Thread.
	TQueue<FSoftObjectPath, EQueueMode::Mpsc> CommonItemDefsPathsQueue;
	ParallelFor(TreasureClasses.Num(), [&CommonItemDefsPathsQueue, &TreasureClasses](int32 Index)
		{
			const FObsidianTreasureClass& TreasureClass = TreasureClasses[Index];
			for (const FObsidianDropItem& DropItem : TreasureClass.DropItems)
			{
				CommonItemDefsPathsQueue.Enqueue(DropItem.SoftTreasureItemDefinitionClass.ToSoftObjectPath());
			}
		});

	TArray<FSoftObjectPath> CommonItemDefsArray;
	CommonItemDefsArray.Reserve(TreasureClasses.Num());
	
	FSoftObjectPath DequeuedPath;
	while (CommonItemDefsPathsQueue.Dequeue(DequeuedPath))
	{
		CommonItemDefsArray.Add(DequeuedPath);
	}
	
	UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(
				CommonItemDefsArray,
				FStreamableDelegate::CreateUObject(this, &ThisClass::OnCommonItemsLoaded)
			);
}

void UObsidianItemDataLoaderSubsystem::OnCommonItemsLoaded()
{
	UE_LOG(LogItemDataLoader, Log, TEXT("Loaded Common Items"));
}
