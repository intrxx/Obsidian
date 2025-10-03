// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

// ~ Project
#include "ObsidianTypes/ObsidianItemTypes.h"

#include "Engine/DataAsset.h"
#include "ObsidianAffixList.generated.h"

class UGameplayEffect;

/**
 * Item Affix definition used in Affix Tables.
 */
USTRUCT()
struct FObsidianDynamicItemAffix
{
	GENERATED_BODY()

public:
	FObsidianDynamicItemAffix()
		: bOverride_MagicItemAffixRollMultiplier(false)
	{}

	explicit operator bool() const
	{
		return AffixTag.IsValid();
	}

	bool operator ==(const FObsidianDynamicItemAffix& Other) const
	{
		return AffixTag == Other.AffixTag;
	}
	
public:
	/** Affix Gameplay Tag Identifier. */
	UPROPERTY(EditDefaultsOnly, Meta = (Categories = "Item.Affix"), Category = "Obsidian")
	FGameplayTag AffixTag = FGameplayTag::EmptyTag;

	//TODO(intrxx) actually set it to the list default
	UPROPERTY(VisibleDefaultsOnly, Category = "Obsidian|Affix")
	EObsidianAffixType AffixType = EObsidianAffixType::None;

	/** Contains Category Tags of Items that this Affix can be applied to. */
	UPROPERTY(EditDefaultsOnly, Meta = (Categories = "Item.Category"), Category = "Obsidian|AcceptedCategories")
	FGameplayTagContainer AcceptedItemCategories = FGameplayTagContainer::EmptyContainer;

	/** Unique addition to the Item Name. */
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Affix")
	FText AffixItemNameAddition = FText();

	/** Row description of the affix. */
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Affix")
	FText AffixDescription = FText();

	/** Minimum Item Level Requirement to roll this affix. */
	UPROPERTY(EditDefaultsOnly, Meta = (ClampMin = "1", ClampMax = "90"), Category = "Obsidian|Affix")
	uint8 MinItemLevelRequirement = 1;

	UPROPERTY(EditAnywhere, meta = (InlineEditConditionToggle), Category = "Obsidian|Affix")
	uint8 bOverride_MagicItemAffixRollMultiplier : 1;

	/** Multiplier for Affix roll when rolled on Magic Items. */
	UPROPERTY(EditDefaultsOnly, Meta = (ClampMin = "0", EditCondition = "bOverride_MagicItemAffixRollMultiplier"), Category = "Obsidian|Affix")
	float MagicItemAffixRollMultiplier = 2.0f;

	/** Gameplay Effect Class to Apply. */
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Affix")
	TSoftClassPtr<UGameplayEffect> SoftGameplayEffectToApply;
	
	/** Value type of affix, if set to Int it will be rounded down. */
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Affix")
	EObsidianAffixValueType AffixValueType = EObsidianAffixValueType::Int;

	/** Possible Affix Ranges to roll from. */
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Affix")
	TArray<FObsidianAffixValue> PossibleAffixRanges;
};

USTRUCT()
struct FObsidianAffixClass
{
	GENERATED_BODY()

public:
	TArray<FObsidianDynamicItemAffix> GetAllAffixesUpToQuality(const int32 UpToTreasureQuality) const;
	TArray<FObsidianDynamicItemAffix> GetAllAffixesUpToQualityForCategory(const int32 UpToTreasureQuality, const FGameplayTag& ForCategory) const;
	
public:
	//TODO(intrxx) Currently not used for anything, keep or delete?
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	FName AffixClassName;

	/** Types of Affixes added to this class. */
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	EObsidianAffixType AffixClassType = EObsidianAffixType::None;

	/** List of actual Item Affixes. */
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	TArray<FObsidianDynamicItemAffix> ItemAffixList;
};

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianAffixList : public UDataAsset
{
	GENERATED_BODY()

public:
	UObsidianAffixList(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void PostInitProperties() override;
	virtual void PostLoad() override;
	
	TArray<FObsidianAffixClass> GetAllAffixClasses() const;
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	TArray<FObsidianAffixClass> AffixClasses;
};
