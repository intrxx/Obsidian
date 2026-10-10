// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "InventoryItems/ItemAffixes/ObsidianAffixList.h"
#include "ObsidianTreasureList.h"

#include "ObsidianItemDataLoaderSubsystem.generated.h"

class UObsidianItemDataDeveloperSettings;
class UObsidianItemDataConfig;
class UObsidianTreasureList;

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianItemDataLoaderSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& InCollection) override;
	virtual void Deinitialize() override;
	
	bool GetAllCommonTreasureClassesUpToQuality(const int32 InUpToTreasureQuality, TArray<FObsidianTreasureClass>& OutTreasureClass) const;
	bool GetAllCommonTreasureClassesUpToQualityForCategory(const int32 InUpToTreasureQuality, TArray<FObsidianTreasureClass>& OutTreasureClass,
		const FGameplayTag& InForCategory) const;
	bool GetAllUniqueOrSetItemsOfBaseItemTypeUpToQuality(const int32 InUpToTreasureQuality, const EObsidianItemRarity InRarityToGet,
		const FGameplayTag& InOfBaseType, FObsidianTreasureClass& OutTreasureClass) const;
	
	bool GetAllAffixesUpToQualityForCategory_DefaultGeneration(const int32 InUpToTreasureQuality, const FGameplayTag& InForCategoryTag,
		const FGameplayTag& InForBaseTypeTag, TArray<FObsidianDynamicItemAffix>& OutPrefixes, TArray<FObsidianDynamicItemAffix>& OutSuffixes,
		TArray<FObsidianDynamicItemAffix>& OutSkillImplicits) const;
	bool GetAllAffixesUpToQualityForCategory_FullGeneration(const int32 InUpToTreasureQuality, const FGameplayTag& InForCategoryTag,
		const FGameplayTag& InForBaseTypeTag, TArray<FObsidianDynamicItemAffix>& OutPrefixes, TArray<FObsidianDynamicItemAffix>& OutSuffixes,
		TArray<FObsidianDynamicItemAffix>& OutImplicits, TArray<FObsidianDynamicItemAffix>& OutSkillImplicits) const;
	bool GetAllAffixesUpToQualityForCategory_NormalItemGeneration(const int32 InUpToTreasureQuality, const FGameplayTag& InForCategoryTag,
		const FGameplayTag& InForBaseTypeTag, TArray<FObsidianDynamicItemAffix>& OutImplicits, TArray<FObsidianDynamicItemAffix>& OutSkillImplicits);
	bool GetAllSkillImplicitsUpToQualityForCategory(const int32 InUpToTreasureQuality, const FGameplayTag& InForCategoryTag,
		const FGameplayTag& InForBaseTypeTag, TArray<FObsidianDynamicItemAffix>& OutSkillImplicits);
	bool GetAllImplicitsUpToQualityForCategory(const int32 InUpToTreasureQuality, const FGameplayTag& InForCategoryTag,
		const FGameplayTag& InForBaseTypeTag, TArray<FObsidianDynamicItemAffix>& OutImplicits);
	bool GetAllPrefixesUpToQualityForCategory(const int32 InUpToTreasureQuality, const FGameplayTag& InForCategoryTag,
		const FGameplayTag& InForBaseTypeTag, TArray<FObsidianDynamicItemAffix>& OutPrefixes);
	bool GetAllSuffixesUpToQualityForCategory(const int32 InUpToTreasureQuality, const FGameplayTag& InForCategoryTag,
		const FGameplayTag& InForBaseTypeTag, TArray<FObsidianDynamicItemAffix>& OutSuffixes);
	
	
	FString GetRandomRareItemNameAddition(const int32 InUpToTreasureQuality, const FGameplayTag& InForItemCategoryTag) const;
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
