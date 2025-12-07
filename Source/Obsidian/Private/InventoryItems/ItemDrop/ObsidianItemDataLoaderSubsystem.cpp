// Copyright 2024 out of sCope team - Michał Ogiński

#include "InventoryItems/ItemDrop/ObsidianItemDataLoaderSubsystem.h"

#include <Engine/AssetManager.h>

#include "InventoryItems/ItemAffixes/ObsidianAffixAbilitySet.h"
#include "InventoryItems/ItemDrop/ObsidianItemDataConfig.h"
#include "InventoryItems/ItemDrop/ObsidianItemDataDeveloperSettings.h"

DEFINE_LOG_CATEGORY(LogItemDataLoader);

void UObsidianItemDataLoaderSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	LoadItemData();
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

	bool bSuccess = false;
	for (const UObsidianTreasureList* TreasureList : ItemDataConfig->CommonTreasureLists)
	{
		if (TreasureList)
		{
			OutTreasureClass.Append(TreasureList->GetAllTreasureClassesUpToQuality(UpToTreasureQuality));
			bSuccess = true;
		}
	}
	return bSuccess;
}

bool UObsidianItemDataLoaderSubsystem::GetAllCommonTreasureClassesUpToQualityForCategory(const int32 UpToTreasureQuality,
	TArray<FObsidianTreasureClass>& OutTreasureClass, const FGameplayTag& ForCategory) const
{
	if (ItemDataConfig == nullptr)
	{
		return false;
	}

	bool bSuccess = false;
	for (const UObsidianTreasureList* TreasureList : ItemDataConfig->CommonTreasureLists)
	{
		if (TreasureList)
		{
			OutTreasureClass.Append(TreasureList->GetTreasureClassesOfQualityWithCategory(UpToTreasureQuality, ForCategory));
			bSuccess = true;
		}
	}
	return bSuccess;
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
		for (const UObsidianTreasureList* TreasureList : ItemDataConfig->UniqueTreasureLists)
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
		for (const UObsidianTreasureList* TreasureList : ItemDataConfig->SetTreasureLists)
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
	const FGameplayTag& ForCategoryTag, TArray<FObsidianDynamicItemAffix>& OutPrefixes, TArray<FObsidianDynamicItemAffix>& OutSuffixes,
	TArray<FObsidianDynamicItemAffix>& OutSkillImplicits) const
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
					case EObsidianAffixType::SkillImplicit:
						{
							OutSkillImplicits.Append(Class.GetAllAffixesUpToQualityForCategory(UpToTreasureQuality, ForCategoryTag));
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

	if (!OutPrefixes.IsEmpty() || !OutSuffixes.IsEmpty())
	{
		return true;
	}
	return false;
}

bool UObsidianItemDataLoaderSubsystem::GetAllAffixesUpToQualityForCategory_FullGeneration(const int32 UpToTreasureQuality,
	const FGameplayTag& ForCategoryTag, TArray<FObsidianDynamicItemAffix>& OutPrefixes, TArray<FObsidianDynamicItemAffix>& OutSuffixes,
	TArray<FObsidianDynamicItemAffix>& OutImplicits, TArray<FObsidianDynamicItemAffix>& OutSkillImplicits) const
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
					case EObsidianAffixType::SkillImplicit:
						{
							OutSkillImplicits.Append(Class.GetAllAffixesUpToQualityForCategory(UpToTreasureQuality, ForCategoryTag));
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

bool UObsidianItemDataLoaderSubsystem::GetAllAffixesUpToQualityForCategory_NormalItemGeneration(const int32 UpToTreasureQuality,
	const FGameplayTag& ForCategoryTag, TArray<FObsidianDynamicItemAffix>& OutImplicits, TArray<FObsidianDynamicItemAffix>& OutSkillImplicits)
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
				case EObsidianAffixType::SkillImplicit:
					{
						OutSkillImplicits.Append(Class.GetAllAffixesUpToQualityForCategory(UpToTreasureQuality, ForCategoryTag));
					} break;
				default:
					{} break;
				}
			}
		}
	}

	if (!OutImplicits.IsEmpty() || !OutSkillImplicits.IsEmpty())
	{
		return true;
	}
	return false;
}

bool UObsidianItemDataLoaderSubsystem::GetAllSkillImplicitsUpToQualityForCategory(const int32 UpToTreasureQuality, const FGameplayTag& ForCategoryTag,
	TArray<FObsidianDynamicItemAffix>& OutSkillImplicits)
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
				if (Class.AffixClassType == EObsidianAffixType::SkillImplicit)
				{
					OutSkillImplicits.Append(Class.GetAllAffixesUpToQualityForCategory(UpToTreasureQuality, ForCategoryTag));
				}
			}
		}
	}

	if (!OutSkillImplicits.IsEmpty())
	{
		return true;
	}
	return false;
}

FString UObsidianItemDataLoaderSubsystem::GetRandomRareItemNameAddition(const int32 UpToTreasureQuality, const FGameplayTag& ForItemCategoryTag) const
{
	if (ItemDataConfig)
	{
		return ItemDataConfig->GetRandomItemNameAddition(UpToTreasureQuality, ForItemCategoryTag);
	}
	return FString();
}

FString UObsidianItemDataLoaderSubsystem::GetAffixMultiplierMagicItemNameAddition() const
{
	if (ItemDataConfig)
	{
		return ItemDataConfig->MultiplierItemNameAddition;
	}
	return FString();
}

void UObsidianItemDataLoaderSubsystem::LoadItemData()
{
	const UObsidianItemDataDeveloperSettings* ItemDataSettings = GetDefault<UObsidianItemDataDeveloperSettings>();
	if (ItemDataSettings == nullptr)
	{
		UE_LOG(LogItemDataLoader, Error, TEXT("ObsidianTreasureConfigDeveloperSettings was not found! Abandoning Loading Item Data Config."));
		return;
	}

	
	const TSoftObjectPtr<UObsidianItemDataConfig>& ItemDataConfigRef = ItemDataSettings->ItemDataConfig;
	const TSoftObjectPtr<UObsidianAffixAbilitySet>& AffixAbilitySet = ItemDataSettings->DefaultAffixAbilitySet;
	TArray<FSoftObjectPath> PathsToLoad;
	if (ItemDataConfigRef.IsNull() == false)
	{
		PathsToLoad.Add(ItemDataConfigRef.ToSoftObjectPath());
	}
	if (AffixAbilitySet.IsNull() == false)
	{
		PathsToLoad.Add(AffixAbilitySet.ToSoftObjectPath());
	}

	UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(
				PathsToLoad,
				FStreamableDelegate::CreateUObject(this, &ThisClass::OnItemDataLoaded)
			);
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
		if (ItemDataSettings->DefaultAffixAbilitySet)
		{
			DefaultAffixAbilitySet = ItemDataSettings->DefaultAffixAbilitySet.Get();
			UE_LOG(LogItemDataLoader, Log, TEXT("Loaded Default Affix Ability Set: [%s]."), *ItemDataConfig->GetName());
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
#if !UE_BUILD_SHIPPING
	UE_LOG(LogItemDataLoader, Log, TEXT("Loaded Common Items"));

	UE_LOG(LogItemDataLoader, Log, TEXT("Treasure Classes Available in the game:"));
	for (const UObsidianTreasureList* TL : ItemDataConfig->CommonTreasureLists)
	{
		if (TL)
		{
			UE_LOG(LogItemDataLoader, Log, TEXT("TL: [%s]"), *GetNameSafe(TL));
			for (const FObsidianTreasureClass& TC : TL->GetAllTreasureClasses())
			{
				UE_LOG(LogItemDataLoader, Log, TEXT("TC:	-[%s]"), *TC.DebugName);
			}
		}
	}
#endif
}
