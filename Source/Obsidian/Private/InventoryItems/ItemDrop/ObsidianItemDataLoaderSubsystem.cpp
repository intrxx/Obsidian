// Copyright 2026 out of sCope team - intrxx

#include "InventoryItems/ItemDrop/ObsidianItemDataLoaderSubsystem.h"

#include "Engine/AssetManager.h"

#include "InventoryItems/ItemAffixes/ObsidianAffixAbilitySet.h"
#include "InventoryItems/ItemDrop/ObsidianItemDataConfig.h"
#include "InventoryItems/ItemDrop/ObsidianItemDataDeveloperSettings.h"
#include "Obsidian/ObsidianLogCategories.h"


void UObsidianItemDataLoaderSubsystem::Initialize(FSubsystemCollectionBase& InCollection)
{
	Super::Initialize(InCollection);

	LoadItemData();
}

void UObsidianItemDataLoaderSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

bool UObsidianItemDataLoaderSubsystem::GetAllCommonTreasureClassesUpToQuality(const int32 InUpToTreasureQuality,
	TArray<FObsidianTreasureClass>& OutTreasureClass) const
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
			OutTreasureClass.Append(TreasureList->GetAllTreasureClassesUpToQuality(InUpToTreasureQuality));
			bSuccess = true;
		}
	}
	return bSuccess;
}

bool UObsidianItemDataLoaderSubsystem::GetAllCommonTreasureClassesUpToQualityForCategory(const int32 InUpToTreasureQuality,
	TArray<FObsidianTreasureClass>& OutTreasureClass, const FGameplayTag& InForCategory) const
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
			OutTreasureClass.Append(TreasureList->GetTreasureClassesOfQualityWithCategory(InUpToTreasureQuality, InForCategory));
			bSuccess = true;
		}
	}
	return bSuccess;
}

bool UObsidianItemDataLoaderSubsystem::GetAllUniqueOrSetItemsOfBaseItemTypeUpToQuality(const int32 InUpToTreasureQuality,
	const EObsidianItemRarity InRarityToGet, const FGameplayTag& InOfBaseType, FObsidianTreasureClass& OutTreasureClass) const
{
	if (ItemDataConfig == nullptr)
	{
		return false;
	}

	if (InRarityToGet == EObsidianItemRarity::Unique)
	{
		for (const UObsidianTreasureList* TreasureList : ItemDataConfig->UniqueTreasureLists)
		{
			if (TreasureList)
			{
				OutTreasureClass = FObsidianTreasureClass(TreasureList->GetAllItemsOfBaseTypeUpToQuality(InUpToTreasureQuality,
					InOfBaseType));
				return true;
			}
		}
	}
	else if (InRarityToGet == EObsidianItemRarity::Set)
	{
		for (const UObsidianTreasureList* TreasureList : ItemDataConfig->SetTreasureLists)
		{
			if (TreasureList)
			{
				OutTreasureClass = FObsidianTreasureClass(TreasureList->GetAllItemsOfBaseTypeUpToQuality(InUpToTreasureQuality,
					InOfBaseType));
				return true;
			}
		}
	}
	return false;
}

bool UObsidianItemDataLoaderSubsystem::GetAllAffixesUpToQualityForCategory_DefaultGeneration(const int32 InUpToTreasureQuality,
	const FGameplayTag& InForCategoryTag, const FGameplayTag& InForBaseTypeTag, TArray<FObsidianDynamicItemAffix>& OutPrefixes,
	TArray<FObsidianDynamicItemAffix>& OutSuffixes, TArray<FObsidianDynamicItemAffix>& OutSkillImplicits) const
{
	if (ItemDataConfig == nullptr)
	{
		return false;
	}

	for (const UObsidianAffixList* AffixLists : ItemDataConfig->CommonAffixLists)
	{
		if (AffixLists)
		{
			for (const FObsidianAffixClass& Class : AffixLists->ReadAllAffixClasses())
			{
				switch (Class.AffixClassType)
				{
					case EObsidianAffixType::SkillImplicit:
						{
							OutSkillImplicits.Append(Class.GetAllAffixesUpToQualityForCategory(InUpToTreasureQuality,
								InForCategoryTag, InForBaseTypeTag));
						} break;
					case EObsidianAffixType::Prefix:
						{
							OutPrefixes.Append(Class.GetAllAffixesUpToQualityForCategory(InUpToTreasureQuality,
								InForCategoryTag, InForBaseTypeTag));
						} break;
					case EObsidianAffixType::Suffix:
						{
							OutSuffixes.Append(Class.GetAllAffixesUpToQualityForCategory(InUpToTreasureQuality,
								InForCategoryTag, InForBaseTypeTag));
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

bool UObsidianItemDataLoaderSubsystem::GetAllAffixesUpToQualityForCategory_FullGeneration(const int32 InUpToTreasureQuality,
	const FGameplayTag& InForCategoryTag, const FGameplayTag& InForBaseTypeTag, TArray<FObsidianDynamicItemAffix>& OutPrefixes,
	TArray<FObsidianDynamicItemAffix>& OutSuffixes, TArray<FObsidianDynamicItemAffix>& OutImplicits,
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
			for (const FObsidianAffixClass& Class : AffixLists->ReadAllAffixClasses())
			{
				switch (Class.AffixClassType)
				{
					case EObsidianAffixType::Implicit:
						{
							OutImplicits.Append(Class.GetAllAffixesUpToQualityForCategory(InUpToTreasureQuality,
								InForCategoryTag, InForBaseTypeTag));
						} break;
					case EObsidianAffixType::SkillImplicit:
						{
							OutSkillImplicits.Append(Class.GetAllAffixesUpToQualityForCategory(InUpToTreasureQuality,
								InForCategoryTag, InForBaseTypeTag));
						} break;
					case EObsidianAffixType::Prefix:
						{
							OutPrefixes.Append(Class.GetAllAffixesUpToQualityForCategory(InUpToTreasureQuality,
								InForCategoryTag, InForBaseTypeTag));
						} break;
					case EObsidianAffixType::Suffix:
						{
							OutSuffixes.Append(Class.GetAllAffixesUpToQualityForCategory(InUpToTreasureQuality,
								InForCategoryTag, InForBaseTypeTag));
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

bool UObsidianItemDataLoaderSubsystem::GetAllAffixesUpToQualityForCategory_NormalItemGeneration(const int32 InUpToTreasureQuality,
	const FGameplayTag& InForCategoryTag, const FGameplayTag& InForBaseTypeTag, TArray<FObsidianDynamicItemAffix>& OutImplicits,
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
			for (const FObsidianAffixClass& Class : AffixLists->ReadAllAffixClasses())
			{
				switch (Class.AffixClassType)
				{
				case EObsidianAffixType::Implicit:
					{
						OutImplicits.Append(Class.GetAllAffixesUpToQualityForCategory(InUpToTreasureQuality,
							InForCategoryTag, InForBaseTypeTag));
					} break;
				case EObsidianAffixType::SkillImplicit:
					{
						OutSkillImplicits.Append(Class.GetAllAffixesUpToQualityForCategory(InUpToTreasureQuality,
							InForCategoryTag, InForBaseTypeTag));
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

bool UObsidianItemDataLoaderSubsystem::GetAllSkillImplicitsUpToQualityForCategory(const int32 InUpToTreasureQuality,
	const FGameplayTag& InForCategoryTag, const FGameplayTag& InForBaseTypeTag, TArray<FObsidianDynamicItemAffix>& OutSkillImplicits)
{
	if (ItemDataConfig == nullptr)
	{
		return false;
	}

	for (const UObsidianAffixList* AffixLists : ItemDataConfig->CommonAffixLists)
	{
		if (AffixLists)
		{
			for (const FObsidianAffixClass& Class : AffixLists->ReadAllAffixClasses())
			{
				if (Class.AffixClassType == EObsidianAffixType::SkillImplicit)
				{
					OutSkillImplicits.Append(Class.GetAllAffixesUpToQualityForCategory(InUpToTreasureQuality,
						InForCategoryTag, InForBaseTypeTag));
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

bool UObsidianItemDataLoaderSubsystem::GetAllImplicitsUpToQualityForCategory(const int32 InUpToTreasureQuality,
	const FGameplayTag& InForCategoryTag, const FGameplayTag& InForBaseTypeTag,
	TArray<FObsidianDynamicItemAffix>& OutImplicits)
{
	if (ItemDataConfig == nullptr)
	{
		return false;
	}

	for (const UObsidianAffixList* AffixLists : ItemDataConfig->CommonAffixLists)
	{
		if (AffixLists)
		{
			for (const FObsidianAffixClass& Class : AffixLists->ReadAllAffixClasses())
			{
				if (Class.AffixClassType == EObsidianAffixType::Implicit)
				{
					OutImplicits.Append(Class.GetAllAffixesUpToQualityForCategory(InUpToTreasureQuality,
						InForCategoryTag, InForBaseTypeTag));
				}
			}
		}
	}

	if (!OutImplicits.IsEmpty())
	{
		return true;
	}
	return false;
}

bool UObsidianItemDataLoaderSubsystem::GetAllPrefixesUpToQualityForCategory(const int32 InUpToTreasureQuality,
	const FGameplayTag& InForCategoryTag, const FGameplayTag& InForBaseTypeTag,
	TArray<FObsidianDynamicItemAffix>& OutPrefixes)
{
	if (ItemDataConfig == nullptr)
	{
		return false;
	}

	for (const UObsidianAffixList* AffixLists : ItemDataConfig->CommonAffixLists)
	{
		if (AffixLists)
		{
			for (const FObsidianAffixClass& Class : AffixLists->ReadAllAffixClasses())
			{
				if (Class.AffixClassType == EObsidianAffixType::Prefix)
				{
					OutPrefixes.Append(Class.GetAllAffixesUpToQualityForCategory(InUpToTreasureQuality,
						InForCategoryTag, InForBaseTypeTag));
				}
			}
		}
	}

	if (!OutPrefixes.IsEmpty())
	{
		return true;
	}
	return false;
}

bool UObsidianItemDataLoaderSubsystem::GetAllSuffixesUpToQualityForCategory(const int32 InUpToTreasureQuality,
	const FGameplayTag& InForCategoryTag, const FGameplayTag& InForBaseTypeTag,
	TArray<FObsidianDynamicItemAffix>& OutSuffixes)
{
	if (ItemDataConfig == nullptr)
	{
		return false;
	}

	for (const UObsidianAffixList* AffixLists : ItemDataConfig->CommonAffixLists)
	{
		if (AffixLists)
		{
			for (const FObsidianAffixClass& Class : AffixLists->ReadAllAffixClasses())
			{
				if (Class.AffixClassType == EObsidianAffixType::Suffix)
				{
					OutSuffixes.Append(Class.GetAllAffixesUpToQualityForCategory(InUpToTreasureQuality,
						InForCategoryTag, InForBaseTypeTag));
				}
			}
		}
	}

	if (!OutSuffixes.IsEmpty())
	{
		return true;
	}
	return false;
}

FString UObsidianItemDataLoaderSubsystem::GetRandomRareItemNameAddition(const int32 InUpToTreasureQuality, const FGameplayTag& InForItemCategoryTag) const
{
	if (ItemDataConfig)
	{
		return ItemDataConfig->GetRandomItemNameAddition(InUpToTreasureQuality, InForItemCategoryTag);
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
		UE_LOG(ObLogItemData, Error, TEXT("ObsidianTreasureConfigDeveloperSettings was not found! Abandoning Loading Item Data Config."));
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
			UE_LOG(ObLogItemData, Log, TEXT("Loaded Treasure Config: [%s]."), *ItemDataConfig->GetName());
		}
		if (ItemDataSettings->DefaultAffixAbilitySet)
		{
			DefaultAffixAbilitySet = ItemDataSettings->DefaultAffixAbilitySet.Get();
			UE_LOG(ObLogItemData, Log, TEXT("Loaded Default Affix Ability Set: [%s]."), *DefaultAffixAbilitySet->GetName());
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
	ParallelFor(TreasureClasses.Num(), [&CommonItemDefsPathsQueue, &TreasureClasses](int32 InIndex)
		{
			const FObsidianTreasureClass& TreasureClass = TreasureClasses[InIndex];
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
	UE_LOG(ObLogItemData, Log, TEXT("Loaded Common Items"));

	UE_LOG(ObLogItemData, Verbose, TEXT("Treasure Classes Available in the game:"));
	for (const UObsidianTreasureList* TL : ItemDataConfig->CommonTreasureLists)
	{
		if (TL)
		{
			UE_LOG(ObLogItemData, Verbose, TEXT("TL: [%s]"), *GetNameSafe(TL));
			for (const FObsidianTreasureClass& TC : TL->GetAllTreasureClasses())
			{
				UE_LOG(ObLogItemData, Verbose, TEXT("TC:	-[%s]"), *TC.DebugName);
			}
		}
	}
#endif
}
