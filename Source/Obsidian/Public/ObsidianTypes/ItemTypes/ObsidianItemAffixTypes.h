// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include <CoreMinimal.h>
#include <GameplayTagContainer.h>
#include <AttributeSet.h>

#include "GameplayEffectTypes.h"
#include "ObsidianItemAffixTypes.generated.h"

struct FObsidianActiveItemAffix;
struct FObsidianDynamicItemAffix;

class UObsidianAffixAbilitySet;
class UGameplayEffect;

UENUM(BlueprintType)
enum class EObsidianAffixValueType : uint8
{
	Int,
	Float
};

/**
 * 
 */
UENUM(BlueprintType)
enum class EObsidianAffixType : uint8
{
	None = 0,
	Prefix,
	Suffix,
	Implicit,
	SkillImplicit,
	Unique
};

UENUM(BlueprintType)
enum class EObsidianItemRarity : uint8
{
	None = 0,
	Quest,
	Normal,
	Magic,
	Rare,
	Unique,
	Set
};

/**
 * 
 */
USTRUCT()
struct FObsidianAffixTier
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Meta = (ClampMin = "1", ClampMax = "8"))
	uint8 AffixTierValue = 1;

	UPROPERTY(EditDefaultsOnly, Meta = (ClampMin = "1", ClampMax = "90"))
	uint8 MinItemLevelRequirement = 1;	
};

/**
 * 
 */
USTRUCT()
struct FObsidianAffixValueRange
{
	GENERATED_BODY()

public:
	FObsidianAffixValueRange(){}

public:
	UPROPERTY(EditDefaultsOnly)
	FObsidianAffixTier AffixTier;

	UPROPERTY(EditDefaultsOnly, meta=(ClampMin = "0", ClampMax = "1000"), Category = "Obsidian")
	uint16 AffixTierWeight = 1000;
	
	/** Array (as there can be multiple values in a single Affix) of Affix Range to Roll. */
	UPROPERTY(EditDefaultsOnly)
	TArray<FFloatRange> AffixRanges;
};

/**
 *
 */
USTRUCT()
struct FObsidianAffixIdentifier
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Meta = (Categories = "Item.AffixValue"))
	FGameplayTag AffixValueID;

	UPROPERTY(EditDefaultsOnly)
	FGameplayAttribute AttributeToModify;
};

/**
 * 
 */
USTRUCT()
struct FObsidianAffixValues
{
	GENERATED_BODY()

public:
	bool IsValid() const;
	
public:
	/** Value type of affix, if set to Int it will be rounded down. */
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Affix")
	EObsidianAffixValueType AffixValueType = EObsidianAffixValueType::Int;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Affix")
	TEnumAsByte<EGameplayModOp::Type> ApplyingRule = EGameplayModOp::AddBase;

	UPROPERTY(EditAnywhere, meta = (InlineEditConditionToggle), Category = "Obsidian|Affix")
	uint8 bOverride_MagicItemAffixRollMultiplier : 1;

	/** Drop only multiplier for Affix roll when rolled on Magic Items. The chance of rolling this increased multiplier is 1/4 now. */
	UPROPERTY(EditDefaultsOnly, Meta = (ClampMin = "1", EditCondition = "bOverride_MagicItemAffixRollMultiplier"), Category = "Obsidian|Affix")
	float MagicItemAffixRollMultiplier = 2.0f;
	
	/**
	 * Array of affix range value identifiers which is mapped 1 to 1 to Affix Range entries and to the corresponding FGameplayAttribute that it modifies,
	 * meaning that the first entry from this array will be a GameplayTag identifier + GameplayAttribute mapped to the first entry in AffixRanges.
	 */
	UPROPERTY(EditDefaultsOnly, Meta = (Categories = "Item.AffixValue"))
	TArray<FObsidianAffixIdentifier> AffixValuesIdentifiers;
	
	/** Affix ranges that will be rolled on the item. Count of AffixRanges must equal provided AffixValuesIdentifiers. */
	UPROPERTY(EditDefaultsOnly)
	TArray<FObsidianAffixValueRange> PossibleAffixRanges;
};

/**
 * 
 */
USTRUCT()
struct FObsidianActiveAffixValue
{
	GENERATED_BODY()

public:
	UPROPERTY()
	uint8 AffixTier = -1;

	UPROPERTY()
	TArray<FObsidianAffixIdentifier> AffixValuesIdentifiers;
	
	UPROPERTY()
	TArray<float> AffixValues;
};

/**
 * Static Item Affix definition used in the designer to define Affixes.
 */
USTRUCT(BlueprintType)
struct FObsidianStaticItemAffix
{
	GENERATED_BODY();

public:
	bool IsEmptyImplicit() const;
	bool IsEmptyAffix() const;

	explicit operator bool() const;
	bool operator ==(const FObsidianStaticItemAffix& Other) const;
	bool operator ==(const FObsidianDynamicItemAffix& Other) const;
	bool operator ==(const FObsidianActiveItemAffix& Other) const;

public:
	/** Unique Affix Gameplay Tag Identifier. */
	UPROPERTY(EditDefaultsOnly, Meta = (Categories = "Item.Affix"), Category = "Obsidian")
	FGameplayTag AffixTag = FGameplayTag::EmptyTag;
	
	UPROPERTY(VisibleDefaultsOnly, Category = "Obsidian|UI")
	EObsidianAffixType AffixType = EObsidianAffixType::Unique;
	
	/** Unique addition to the Item Name. */
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|UI")
	FString AffixItemNameAddition = FString();

	/** Row description of the affix. */
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|UI")
	FText AffixDescription = FText();

	/** Soft Ability Set to Apply, usually it will be just Gameplay Effect. */
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Affix")
	TSoftObjectPtr<UObsidianAffixAbilitySet> SoftAbilitySetToApply = nullptr;
	
	/** Affix ranges. */
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Affix")
	FObsidianAffixValues AffixValuesDefinition = FObsidianAffixValues();
};

/**
 * Item Affix definition used in Affix Tables.
 */
USTRUCT()
struct FObsidianDynamicItemAffix
{
	GENERATED_BODY()

public:
	FObsidianDynamicItemAffix(){}

	explicit operator bool() const;
	bool operator ==(const FObsidianDynamicItemAffix& Other) const;
	bool operator ==(const FObsidianActiveItemAffix& Other) const;
	bool operator ==(const FObsidianStaticItemAffix& Other) const;
	
public:
	/** Unique Affix Gameplay Tag Identifier. */
	UPROPERTY(EditDefaultsOnly, Meta = (Categories = "Item.Affix"), Category = "Obsidian")
	FGameplayTag AffixTag = FGameplayTag::EmptyTag;

	UPROPERTY(EditDefaultsOnly, Meta = (ClampMin = "1", ClampMax = "1000"), Category = "Obsidian")
	uint16 AffixWeight = 1000;
	
	UPROPERTY(VisibleDefaultsOnly, Category = "Obsidian|Affix")
	EObsidianAffixType AffixType = EObsidianAffixType::None;

	/** Contains Category Tags of Items that this Affix can be applied to. */
	UPROPERTY(EditDefaultsOnly, Meta = (Categories = "Item.Category"), Category = "Obsidian|AcceptedCategories")
	FGameplayTagContainer AcceptedItemCategories = FGameplayTagContainer::EmptyContainer;

	/** Unique addition to Magic Item Name based on Affixes. */
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Affix")
	FString AffixItemNameAddition = FString();
	
	/** Row description of the affix. */
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Affix")
	FText AffixDescription = FText();

	/** Minimum Item Level Requirement to roll this affix. */
	UPROPERTY(EditDefaultsOnly, Meta = (ClampMin = "1", ClampMax = "90"), Category = "Obsidian|Affix")
	uint8 MinItemLevelRequirement = 1;
	
	/** Soft Ability Set to Apply, usually it will be just Gameplay Effect. */
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Affix")
	TSoftObjectPtr<UObsidianAffixAbilitySet> SoftAbilitySetToApply = nullptr;
	
	/** Possible Affix Ranges to roll from. */
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Affix")
	FObsidianAffixValues AffixValuesDefinition = FObsidianAffixValues();
};

/**
 * Affix that has been added to the Item.
 */
USTRUCT()
struct FObsidianActiveItemAffix
{
	GENERATED_BODY()

public:
	FObsidianActiveItemAffix(){};
	
	explicit operator bool() const
	{
		return AffixTag.IsValid();
	}

	bool operator==(const FObsidianActiveItemAffix& Other) const;
	bool operator==(const FObsidianDynamicItemAffix& Other) const;
	bool operator==(const FObsidianStaticItemAffix& Other) const;

	uint8 GetCurrentAffixTier() const;
	
	void InitializeWithDynamic(const FObsidianDynamicItemAffix& InDynamicItemAffix, const uint8 UpToTreasureQuality, const bool bApplyMultiplier);
	void InitializeWithStatic(const FObsidianStaticItemAffix& InStaticItemAffix, const uint8 UpToTreasureQuality, const bool bApplyMultiplier);
	
	void RandomizeAffixValueBoundByRange();
public:
	UPROPERTY()
	FGameplayTag AffixTag = FGameplayTag::EmptyTag;

	UPROPERTY()
	EObsidianAffixType AffixType = EObsidianAffixType::None;
	
	UPROPERTY()
	FString AffixItemNameAddition = FString();
	
	UPROPERTY()
	FText ActiveAffixDescription = FText();

	UPROPERTY()
	FText UnformattedAffixDescription = FText();
	
	UPROPERTY()
	TSoftObjectPtr<UObsidianAffixAbilitySet> SoftAbilitySetToApply;
	
	UPROPERTY()
	FObsidianAffixValues AffixValuesDefinition = FObsidianAffixValues();

	UPROPERTY()
	FObsidianActiveAffixValue CurrentAffixValue = FObsidianActiveAffixValue();

private:
	void CreateAffixActiveDescription();
	void InitializeAffixTierAndRange(const uint8 UpToTreasureQuality, const bool bApplyMultiplier);
	FObsidianAffixValueRange GetRandomAffixRange(const uint8 UpToTreasureQuality);
};

/**
 * 
 */
USTRUCT(BlueprintType)
struct FObsidianStacksUIData
{
public:
	GENERATED_BODY();
	
	FObsidianStacksUIData(){}
	FObsidianStacksUIData(const int32 CurrentStacks, const int32 MaxStacks)
		: CurrentItemStackCount(CurrentStacks)
		, MaxItemStackCount(MaxStacks)
	{}

	void SetCurrentStacks(const int32 InCurrentStacks)
	{
		CurrentItemStackCount = InCurrentStacks;
	}

	void SetMaxStacks(const int32 InMaxStacks)
	{
		MaxItemStackCount = InMaxStacks;
	}
	
public:
	UPROPERTY(BlueprintReadOnly)
	int32 CurrentItemStackCount = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 MaxItemStackCount = 0;
};

/**
 * 
 */
USTRUCT(BlueprintType)
struct FObsidianAffixDescriptionRow
{
	GENERATED_BODY()
public:
	void SetAffixAdditionalDescription(const EObsidianAffixType& InAffixType, const int32 InAffixTier);
	
public:
	FGameplayTag AffixTag = FGameplayTag::EmptyTag;
	EObsidianAffixType AffixType = EObsidianAffixType::None;
	
	FText AffixRowDescription = FText();
	FText AffixAdditionalDescription = FText();
	FString AffixItemNameAddition = FString();
	FString Affix = FString();
};

/**
 * 
 */
USTRUCT(BlueprintType)
struct FObsidianRareItemNameAddition
{
	GENERATED_BODY()
	
public:
	/** Range of item level that this name can be generated for. */
	UPROPERTY(EditDefaultsOnly, Meta = (ClampMin = 1, ClampMax = 90), Category = "Obsidian")
	FIntPoint ItemLevelRange = FIntPoint(1, 90);
	
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	TArray<FText> ItemNameAdditions;
};

/**
 * 
 */
USTRUCT(BlueprintType)
struct FObsidianRareItemSuffixNameAddition
{
	GENERATED_BODY()

public:
	/** Category of items that this suffix name addition can be applied to. */
	UPROPERTY(EditDefaultsOnly, Meta = (Categories = "Item.Category"), Category = "Obsidian")
	FGameplayTagContainer ForItemCategories = FGameplayTagContainer::EmptyContainer;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	TArray<FObsidianRareItemNameAddition> ItemNameAdditions;
};

USTRUCT()
struct FObsidianRareItemNameGenerationData
{
	GENERATED_BODY()

public:
	FText GetRandomPrefixNameAddition(const int32 UpToTreasureQuality);
	FText GetRandomSuffixNameAddition(const int32 UpToTreasureQuality, const FGameplayTag& ForItemCategory);
	
public:
	/** General prefix item name additions. */
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	TArray<FObsidianRareItemNameAddition> PrefixNameAdditions;

	/** Unique per category item suffix name additions. */
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	TArray<FObsidianRareItemSuffixNameAddition> SuffixNameAdditions;
};

USTRUCT(BlueprintType)
struct FObsidianItemStats
{
	GENERATED_BODY()

public:
	bool ContainsItemImage() const
	{
		return bContainsItemImage;
	}
	
	/** Checks if Item Stats contain Item Display Name. This should technically be present on every Item Stats. */
	bool ContainsDisplayName() const
	{
		return bContainsDisplayName;
	}

	/** Checks if Item Stats contain Rare Item Display Name Addition. */
	bool ContainsRareDisplayNameAddition() const
	{
		return bContainsRareItemDisplayNameAddition;
	}

	/** Checks if Item Stats contain Magic Item Display Name Addition. */
	bool ContainsMagicDisplayNameAddition() const
	{
		return bContainsMagicItemDisplayNameAddition;
	}

	/** Checks if the Item Stats contain Description. */
	bool ContainsDescription() const
	{
		return bContainsDescription;
	}

	/** Checks if the Item Stats contain Additional Description. */
	bool ContainsAdditionalDescription() const
	{
		return bContainsAdditionalDescription;
	}

	/** Checks if the Item Stats contain Stacks info. */
	bool ContainsStacks() const
	{
		return bContainsStacks;
	}

	/** Checks if the Item Stats contain Affixes info. */
	bool ContainsAffixes() const
	{
		return bContainsAffixes;
	}

	/** Checks if the Item supports Identification. */
	bool SupportsIdentification() const
	{
		return bSupportIdentification;
	}

	UTexture2D* GetItemImage() const
	{
		return ItemImage;
	}

	FIntPoint GetItemGridSpan() const
	{
		return ItemGridSpan;
	}

	FText GetDisplayName() const
	{
		return DisplayName;
	}

	FString GetRareItemDisplayNameAddition() const
	{
		return RareItemDisplayNameAddition;
	}

	FString GetMagicItemDisplayNameAddition() const
	{
		return MagicItemDisplayNameAddition;
	}

	FText GetDescription() const
	{
		return Description;
	}

	FText GetAdditionalDescription() const
	{
		return AdditionalDescription;
	}

	FObsidianStacksUIData GetItemStacks() const
	{
		return StacksData;
	}

	bool IsIdentified() const
	{
		return bIdentified;
	}

	TArray<FObsidianAffixDescriptionRow> GetAffixDescriptions() const
	{
		return AffixDescriptionRows;
	}

	/** It takes an additional ItemGridSpan argument as UI needs it to rescale the image for now. */
	void SetItemImage(UTexture2D* InItemImage, const FIntPoint& InItemGridSpan)
	{
		bContainsItemImage = true;
		ItemImage = InItemImage;
		ItemGridSpan = InItemGridSpan;
	}
	
	void SetDisplayName(const FText& InDisplayName)
	{
		bContainsDisplayName = true;
		DisplayName = InDisplayName;
	}

	void SetRareDisplayNameAddition(const FString& InDisplayNameAddition)
	{
		if (InDisplayNameAddition.IsEmpty() == false) // This will be empty for anything other than Rare or Magic item.
		{
			bContainsRareItemDisplayNameAddition = true;
			RareItemDisplayNameAddition = InDisplayNameAddition;
		}
	}
	
	void SetMagicDisplayNameAddition(const FString& InDisplayNameAddition)
	{
		if (InDisplayNameAddition.IsEmpty() == false) // This will be empty for anything other than Rare or Magic item.
		{
			bContainsMagicItemDisplayNameAddition = true;
			MagicItemDisplayNameAddition = InDisplayNameAddition;
		}
	}

	void SetDescription(const FText& InDescription)
	{
		bContainsDescription = true;
		Description = InDescription;
	}

	void SetAdditionalDescription(const FText& InAdditionalDescription)
	{
		bContainsAdditionalDescription = true;
		AdditionalDescription = InAdditionalDescription;
	}

	void SetStacks(const int32 InCurrentStack, const int32 InMaxStacks)
	{
		bContainsStacks = true;
		StacksData = FObsidianStacksUIData(InCurrentStack, InMaxStacks);
	}

	void SetCurrentStacks(const int32 InCurrentStack)
	{
		bContainsStacks = true;
		StacksData.SetCurrentStacks(InCurrentStack);
	}

	void SetMaxStacks(const int32 InMaxStacks)
	{
		bContainsStacks = true;
		StacksData.SetMaxStacks(InMaxStacks);
	}

	void SetIdentified(const bool InIdentified)
	{
		bSupportIdentification = true;
		bIdentified = InIdentified;
	}

	void SetAffixDescriptionRows(const TArray<FObsidianAffixDescriptionRow>& AffixRows)
	{
		bContainsAffixes = true;
		AffixDescriptionRows = AffixRows;
	}
	
public:
	UPROPERTY()
	EObsidianItemRarity ItemRarity = EObsidianItemRarity::Normal;
	
private:
	UPROPERTY()
	TObjectPtr<UTexture2D> ItemImage;

	UPROPERTY()
	FIntPoint ItemGridSpan = FIntPoint::NoneValue;
	
	/**
	 * Item Descriptors.
	 */

	UPROPERTY()
	FText DisplayName = FText::GetEmpty();

	UPROPERTY()
	FString RareItemDisplayNameAddition = FString();

	UPROPERTY()
	FString MagicItemDisplayNameAddition = FString();

	UPROPERTY()
	FText Description = FText::GetEmpty();

	UPROPERTY()
	FText AdditionalDescription = FText::GetEmpty();
	
	/**
	 * Stacks.
	 */

	UPROPERTY()
	FObsidianStacksUIData StacksData = FObsidianStacksUIData();

	/**
	 * Affixes.
	 */

	UPROPERTY()
	TArray<FObsidianAffixDescriptionRow> AffixDescriptionRows;

	bool bIdentified = false;

	/**
	 * Contains booleans.
	 */

	bool bContainsItemImage = false;
	bool bContainsDisplayName = false;
	bool bContainsRareItemDisplayNameAddition = false;
	bool bContainsMagicItemDisplayNameAddition = false;
	bool bContainsDescription = false;
	bool bContainsAdditionalDescription = false;
	bool bContainsStacks = false;
	bool bContainsAffixes = false;
	bool bSupportIdentification = false;
};


