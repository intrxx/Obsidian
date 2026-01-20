// Copyright 2026 out of sCope team - intrxx

#pragma once

#include <CoreMinimal.h>

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
	bool GetAllCommonTreasureClassesUpToQualityForCategory(const int32 UpToTreasureQuality, TArray<FObsidianTreasureClass>& OutTreasureClass,
		const FGameplayTag& ForCategory) const;
	bool GetAllUniqueOrSetItemsOfBaseItemTypeUpToQuality(const int32 UpToTreasureQuality, const EObsidianItemRarity RarityToGet,
		const FGameplayTag& OfBaseType, FObsidianTreasureClass& OutTreasureClass) const;
	
	bool GetAllAffixesUpToQualityForCategory_DefaultGeneration(const int32 UpToTreasureQuality, const FGameplayTag& ForCategoryTag,
		TArray<FObsidianDynamicItemAffix>& OutPrefixes, TArray<FObsidianDynamicItemAffix>& OutSuffixes, TArray<FObsidianDynamicItemAffix>& OutSkillImplicits) const;
	bool GetAllAffixesUpToQualityForCategory_FullGeneration(const int32 UpToTreasureQuality, const FGameplayTag& ForCategoryTag,
		TArray<FObsidianDynamicItemAffix>& OutPrefixes, TArray<FObsidianDynamicItemAffix>& OutSuffixes, TArray<FObsidianDynamicItemAffix>& OutImplicits,
		TArray<FObsidianDynamicItemAffix>& OutSkillImplicits) const;
	bool GetAllAffixesUpToQualityForCategory_NormalItemGeneration(const int32 UpToTreasureQuality, const FGameplayTag& ForCategoryTag,
		TArray<FObsidianDynamicItemAffix>& OutImplicits, TArray<FObsidianDynamicItemAffix>& OutSkillImplicits);
	bool GetAllSkillImplicitsUpToQualityForCategory(const int32 UpToTreasureQuality, const FGameplayTag& ForCategoryTag,
		TArray<FObsidianDynamicItemAffix>& OutSkillImplicits);
	
	
	FString GetRandomRareItemNameAddition(const int32 UpToTreasureQuality, const FGameplayTag& ForItemCategoryTag) const;
	FString GetAffixMultiplierMagicItemNameAddition() const;

public:
	UPROPERTY()
	TObjectPtr<UObsidianAffixAbilitySet> DefaultAffixAbilitySet;
	
protected:
	void LoadItemData();
	void OnItemDataLoaded();
	void OnCommonItemsLoaded();
	
protected:
	UPROPERTY()
	TObjectPtr<UObsidianItemDataConfig> ItemDataConfig;
};
