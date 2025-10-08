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

bool UObsidianItemDataLoaderSubsystem::GetAllCommonTreasureClassesUpToQuality(const int32 UpToTreasureQuality, TArray<FObsidianTreasureClass>& OutTreasureClass) const
{
	if (ItemDataConfig == nullptr)
	{
		return false;
	}
	
	for (UObsidianTreasureList* TreasureList : ItemDataConfig->CommonTreasureLists)
	{
		if (TreasureList)
		{
			OutTreasureClass.Append(TreasureList->GetAllTreasureClassesUpToQuality(UpToTreasureQuality));
			return true;
		}
	}
	return false;
}

bool UObsidianItemDataLoaderSubsystem::GetAllUniqueOrSetItemsOfBaseItemTypeUpToQuality(const int32 UpToTreasureQuality,
	const EObsidianItemRarity RarityToGet, const FGameplayTag& OfBaseType, FObsidianTreasureClass& OutTreasureClass) const
{
	if (ItemDataConfig == nullptr)
	{
		return false;
	}

	if (RarityToGet == EObsidianItemRarity::Unique)
	{
		for (UObsidianTreasureList* TreasureList : ItemDataConfig->UniqueTreasureLists)
		{
			if (TreasureList)
			{
				OutTreasureClass = FObsidianTreasureClass(TreasureList->GetAllItemsOfBaseTypeUpToQuality(UpToTreasureQuality, OfBaseType));
				return true;
			}
		}
	}
	else if (RarityToGet == EObsidianItemRarity::Set)
	{
		for (UObsidianTreasureList* TreasureList : ItemDataConfig->SetTreasureLists)
		{
			if (TreasureList)
			{
				OutTreasureClass = FObsidianTreasureClass(TreasureList->GetAllItemsOfBaseTypeUpToQuality(UpToTreasureQuality, OfBaseType));
				return true;
			}
		}
	}
	return false;
}

bool UObsidianItemDataLoaderSubsystem::GetAllAffixesUpToQualityForCategory_DefaultGeneration(const int32 UpToTreasureQuality,
	const FGameplayTag& ForCategoryTag, TArray<FObsidianDynamicItemAffix>& OutPrefixes, TArray<FObsidianDynamicItemAffix>& OutSuffixes) const
{
	if (ItemDataConfig == nullptr)
	{
		return false;
	}

	for (const UObsidianAffixList* AffixLists : ItemDataConfig->CommonAffixLists)
	{
		if (AffixLists)
		{
			for (const FObsidianAffixClass& Class : AffixLists->GetAllAffixClasses())
			{
				switch (Class.AffixClassType)
				{
					case EObsidianAffixType::Prefix:
						{
							OutPrefixes.Append(Class.GetAllAffixesUpToQualityForCategory(UpToTreasureQuality, ForCategoryTag));
						} break;
					case EObsidianAffixType::Suffix:
						{
							OutSuffixes.Append(Class.GetAllAffixesUpToQualityForCategory(UpToTreasureQuality, ForCategoryTag));
						} break;
						default:
						{} break;
				}
			}
		}
	}

	if (!OutPrefixes.IsEmpty() || !OutSuffixes.IsEmpty())
	{
		return true;
	}
	return false;
}

bool UObsidianItemDataLoaderSubsystem::GetAllAffixesUpToQualityForCategory_FullGeneration(const int32 UpToTreasureQuality,
	const FGameplayTag& ForCategoryTag, TArray<FObsidianDynamicItemAffix>& OutPrefixes, TArray<FObsidianDynamicItemAffix>& OutSuffixes,
	TArray<FObsidianDynamicItemAffix>& OutImplicits) const
{
	if (ItemDataConfig == nullptr)
	{
		return false;
	}
	
	for (const UObsidianAffixList* AffixLists : ItemDataConfig->CommonAffixLists)
	{
		if (AffixLists)
		{
			for (const FObsidianAffixClass& Class : AffixLists->GetAllAffixClasses())
			{
				switch (Class.AffixClassType)
				{
					case EObsidianAffixType::Implicit:
						{
							OutImplicits.Append(Class.GetAllAffixesUpToQualityForCategory(UpToTreasureQuality, ForCategoryTag));
						} break;
					case EObsidianAffixType::Prefix:
						{
							OutPrefixes.Append(Class.GetAllAffixesUpToQualityForCategory(UpToTreasureQuality, ForCategoryTag));
						} break;
					case EObsidianAffixType::Suffix:
						{
							OutSuffixes.Append(Class.GetAllAffixesUpToQualityForCategory(UpToTreasureQuality, ForCategoryTag));
						} break;
						default:
							{} break;
				}
			}
		}
	}

	if (!OutImplicits.IsEmpty() || !OutPrefixes.IsEmpty() || !OutSuffixes.IsEmpty())
	{
		return true;
	}
	return false;
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
