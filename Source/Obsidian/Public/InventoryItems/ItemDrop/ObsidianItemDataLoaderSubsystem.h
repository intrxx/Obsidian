// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project
#include "GameplayTagContainer.h"
#include "ObsidianTreasureList.h"
#include "InventoryItems/ItemAffixes/ObsidianAffixList.h"

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
	
	bool GetAllCommonTreasureClassesUpToQuality(const int32 UpToTreasureQuality, TArray<FObsidianTreasureClass>& OutTreasureClass) const;
	bool GetAllUniqueOrSetItemsOfBaseItemTypeUpToQuality(const int32 UpToTreasureQuality, const EObsidianItemRarity RarityToGet, const FGameplayTag& OfBaseType, FObsidianTreasureClass& OutTreasureClass) const;
	
	bool GetAllAffixesUpToQualityForCategory_DefaultGeneration(const int32 UpToTreasureQuality, const FGameplayTag& ForCategoryTag, TArray<FObsidianDynamicItemAffix>& OutPrefixes, TArray<FObsidianDynamicItemAffix>& OutSuffixes) const;
	bool GetAllAffixesUpToQualityForCategory_FullGeneration(const int32 UpToTreasureQuality, const FGameplayTag& ForCategoryTag, TArray<FObsidianDynamicItemAffix>& OutPrefixes, TArray<FObsidianDynamicItemAffix>& OutSuffixes, TArray<FObsidianDynamicItemAffix>& OutImplicits) const;

	FString GetRandomRareItemNameAddition(const int32 UpToTreasureQuality, const FGameplayTag& ForItemCategoryTag) const;
	
protected:
	void LoadItemDataConfig();
	void OnItemConfigLoaded();
	void OnCommonItemsLoaded();
	
protected:
	UPROPERTY()
	TObjectPtr<UObsidianItemDataConfig> ItemDataConfig;
};
