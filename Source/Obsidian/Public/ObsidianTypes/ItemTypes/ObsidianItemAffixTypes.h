// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Obsidian/ObsidianGameplayTags.h"

// ~ Project

#include "ObsidianItemAffixTypes.generated.h"

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
	Unique
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
	uint8 AffixTier = 1;

	UPROPERTY(EditDefaultsOnly, Meta = (ClampMin = "1", ClampMax = "90"))
	uint8 MinItemLevelRequirement = 1;	
};

/**
 * 
 */
USTRUCT()
struct FObsidianAffixValue
{
	GENERATED_BODY()

public:
	FObsidianAffixValue(){}
	FObsidianAffixValue(const TArray<FFloatRange>& InRange)
		: AffixRanges(InRange)
	{}
	
public:
	UPROPERTY(EditDefaultsOnly)
	FObsidianAffixTier AffixTier;

	/** Array (as there can be multiple values in a single Affix) of Affix Range to Roll. */
	UPROPERTY(EditDefaultsOnly)
	TArray<FFloatRange> AffixRanges;

	UPROPERTY()
	TArray<float> CurrentAffixValues;
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

	explicit operator bool() const
	{
		return AffixTag.IsValid();
	}

public:
	/** Affix Gameplay Tag Identifier. */
	UPROPERTY(EditDefaultsOnly, Meta = (Categories = "Item.Affix"), Category = "Obsidian")
	FGameplayTag AffixTag = FGameplayTag::EmptyTag;
	
	UPROPERTY(VisibleDefaultsOnly, Category = "Obsidian|UI")
	EObsidianAffixType AffixType = EObsidianAffixType::Unique;
	
	/** Unique addition to the Item Name. */
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|UI")
	FText AffixItemNameAddition = FText();

	/** Row description of the affix. */
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|UI")
	FText AffixDescription = FText();

	/** Gameplay Effect Class to Apply. */
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Affix")
	TSoftClassPtr<UGameplayEffect> SoftGameplayEffectToApply;
	
	/** Value type of affix, if set to Int it will be rounded down. */
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Affix")
	EObsidianAffixValueType AffixValueType = EObsidianAffixValueType::Int;
	
	/** Affix ranges. */
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Affix")
	TArray<FFloatRange> PossibleAffixRanges;
};

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

	void InitializeWithDynamic(const FObsidianDynamicItemAffix& InDynamicItemAffix);
	void InitializeWithStatic(const FObsidianStaticItemAffix& InStaticItemAffix);

	void InitializeAffixTierAndRange();
	void RandomizeAffixValue();

public:
	UPROPERTY()
	FGameplayTag AffixTag = FGameplayTag::EmptyTag;

	UPROPERTY()
	EObsidianAffixType AffixType = EObsidianAffixType::None;
	
	UPROPERTY()
	FText AffixItemNameAddition = FText();
	
	UPROPERTY()
	FText AffixDescription = FText();
	
	UPROPERTY()
	TSoftClassPtr<UGameplayEffect> SoftGameplayEffectToApply;

	UPROPERTY()
	EObsidianAffixValueType AffixValueType = EObsidianAffixValueType::Int;
	
	UPROPERTY()
	TArray<FObsidianAffixValue> PossibleAffixRanges;

	UPROPERTY()
	FObsidianAffixValue CurrentAffixValue;
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
	void SetAffixRowDescription(const FText& InAffixDescription, const int32 InTempMagnitude);
	void SetAffixAdditionalDescription(const EObsidianAffixType& InAffixType, const int32 InAffixTier);
	
public:
	FGameplayTag AffixTag = FGameplayTag::EmptyTag;
	EObsidianAffixType AffixType = EObsidianAffixType::None;
	
	FText AffixRowDescription = FText();
	FText AffixAdditionalDescription = FText();
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
	FGameplayTag ItemRarity = ObsidianGameplayTags::Item_Rarity_Normal;
	
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
	bool bContainsDescription = false;
	bool bContainsAdditionalDescription = false;
	bool bContainsStacks = false;
	bool bContainsAffixes = false;
	bool bSupportIdentification = false;
};


