// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include <CoreMinimal.h>
#include <AttributeSet.h>
#include <GameplayEffectTypes.h>

#include "Obsidian/ObsidianGameplayTags.h"
#include "ObsidianTypes/ObsidianCoreTypes.h"

#include "ObsidianItemTypes.generated.h"

struct FObsidianActiveItemAffix;
struct FObsidianDynamicItemAffix;

class UObsidianAffixAbilitySet;
class UObsidianInventoryItemDefinition;
class UObsidianInventoryItemInstance;
class UGameplayEffect;

/**
 * Enum that holds the Rarity of Objects the Player interacts with to drop items mainly enemies and stashes placed in the level.
 */
UENUM()
enum class EObsidianEntityRarity : uint8
{
	None = 0 UMETA(DisplayName = "None"),

	/** This entity is roll logic ambiguous, can be used for quest item spawners to just drop one specific item. */
	Neutral UMETA(DisplayName = "Neutral"),

	/** Normal Enemies, the default rarity of monsters and Item Spawners. */
	Normal UMETA(DisplayName = "Normal"),

	/** Primary used for Chests, slighly righer chance to get items. */
	Magic UMETA(DisplayName = "Magic"),

	/** Used for special packs of monsters with extra loot. */
	Rare UMETA(DisplayName = "Rare"),

	/** Used for companions of Mini Bosses, slightly more loot than Rare enemies. */
	MiniBossCompanion UMETA(DisplayName = "Mini Boss Companion"),

	/** Used for Mini Bosses in the level, high loot. */
	MiniBoss UMETA(DisplayName = "Mini Boss"),

	/** Used for Special Act/Area Bosses. */
	SpecialBoss UMETA(DisplayName = "Special Boss"),
};

/**
 * 
 */
namespace ObsidianInventoryItemsStatics
{
	static const FVector2D InventorySlotSize(64.0f, 64.0f); // TODO Get rid of later
}

/**
 * 
 */
namespace ObsidianDefaultStackCounts 
{
	static constexpr int32 CurrentStackCount = 1;
	static constexpr int32 MaxStackCount = 1;
	static constexpr int32 LimitCount = 0;

	/** Returns the unified project default item stack count for a given Stack Tag. */
	inline int32 GetUnifiedDefaultForTag(const FGameplayTag Tag)
	{
		if(Tag == ObsidianGameplayTags::Item_StackCount_Current)
		{
			return CurrentStackCount;
		}
		if(Tag == ObsidianGameplayTags::Item_StackCount_Max)
		{
			return MaxStackCount;
		}
		if(Tag == ObsidianGameplayTags::Item_StackCount_Limit)
		{
			return LimitCount;
		}
		
		UE_LOG(LogTemp, Error, TEXT("Provided Stack Count Gameplay Tag is invalid, returning 0."));
		return 0;
	}
}

/**
 * Grid Owner.
 */
UENUM()
enum class EObsidianGridOwner : uint8
{
	None = 0,
	Inventory,
	PlayerStash
};

/**
 * 
 */
UENUM(BlueprintType)
enum class EObsidianEquipCheckResult : uint8
{
	None = 0 UMETA(DisplayName="None"),
	
	/** Item cannot be equipped to this Equipment Slot either because it does not fit or this item category was banned by a gameplay mechanic. */
	CannotEquipToSlot UMETA(DisplayName="Cannot Equip To Slot"),
	
	/** Item cannot be equipped, it does not have equipment fragment. */
	ItemUnequippable UMETA(DisplayName="Item Unequippable"),

	/** Player cannot modify the state of the equipment. */
	EquipmentActionsBlocked UMETA(DisplayName="Equipment Actions Blocked"),

	/** Item cannot be equipped, trying to equip two-handed weapon, two slots are occupied and no sufficient space in inventory. */
	UnableToEquip_NoSufficientInventorySpace UMETA(DisplayName="Unable To Equip - No Sufficient Inventory Space"),
	
	/** Item cannot be equipped, item cannot be equipped together with the item in sister slot. */
	UnableToEquip_DoesNotFitWithOtherWeaponType UMETA(DisplayName="Unable To Equip - Item Does not fit with other weapon type."),
	
	/** Item cannot be equipped, it needs to be identified first. */
	ItemUnientified UMETA(DisplayName="Item Unientified"),

	/** Item cannot be equipped, wrong hero class. */
	WrongHeroClass UMETA(DisplayName="Wrong Hero Class"),
	
	/** Item cannot be equipped, no sufficient hero level. */
	HeroLevelTooLow UMETA(DisplayName="Hero Level Too Low"),

	/** Item cannot be equipped, no sufficient Dexterity. */
	NotEnoughDexterity UMETA(DisplayName="Not Enough Dexterity"),

	/** Item cannot be equipped, no sufficient Intelligence. */
	NotEnoughIntelligence UMETA(DisplayName="Not Enough Intelligence"),

	/** Item cannot be equipped, no sufficient Strength. */
	NotEnoughStrength UMETA(DisplayName="Not Enough Strength"),

	/** Item cannot be equipped, no sufficient Faith. */
	NotEnoughFaith UMETA(DisplayName="Not Enough Faith"),

	/** Item can be quipped. */
	CanEquip UMETA(DisplayName="Can Equip")
};

UENUM(BlueprintType)
enum class EObsidianPlacingAtSlotResult : uint8
{
	UnableToPlace_UnfitForCategory UMETA(DisplayName="Item Unfit For Category"),
	UnableToPlace_BannedCategory UMETA(DisplayName="Item is of Banned Category"),
	UnableToPlace_BaseTypeDiffers UMETA(DisplayName="Base Type Differs"),
	CanPlace UMETA(DisplayName="Can Place")
};

UENUM(BlueprintType)
enum class EObsidianItemRarity : uint8
{
	None = 0,
	Quest = 1,
	Normal = 2,
	Magic = 3,
	Rare = 4,
	Unique = 5,
	Set = 6
};

/**
 *
 */
USTRUCT(BlueprintType)
struct FObsidianAttributeRequirement
{
	GENERATED_BODY()

public:
	FObsidianAttributeRequirement(){}
	FObsidianAttributeRequirement(const FGameplayAttribute& InAttribute, const uint16 InMagnitude)
		: RequiredAttribute(InAttribute)
		, RequiredAttributeMagnitude(InMagnitude)
	{}
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "Attribute")
	FGameplayAttribute RequiredAttribute;

	UPROPERTY(EditDefaultsOnly, Category = "Magnitude")
	uint16 RequiredAttributeMagnitude = 0;
};

/**
 *
 */
USTRUCT(BlueprintType)
struct FObsidianItemRequirements
{
	GENERATED_BODY()
	
public:
	FObsidianItemRequirements();
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "Requirement")
	EObsidianHeroClass HeroClassRequirement = EObsidianHeroClass::None;

	UPROPERTY(EditDefaultsOnly, Category = "Requirement")
	TArray<FObsidianAttributeRequirement> AttributeRequirements;

	UPROPERTY(EditDefaultsOnly, Meta = (ClampMin = 0, ClampMax = 90), Category = "Requirement")
	uint8 RequiredLevel = 0;
	
	uint8 bHasAnyRequirements:1;

	/** This flag gets set when the Requirements get initialized and adjusted in Item drop logic. */ 
	uint8 bInitialized:1;
};

/**
 * 
 */
USTRUCT(BlueprintType)
struct FObsidianItemInteractionFlags
{
	GENERATED_BODY()

public:
	FObsidianItemInteractionFlags()
		: bMoveBetweenNextOpenedWindow(false)
		, bAutomaticallyAddToWindow(false)
		, bItemStacksInteraction(false)
	{}

public:
	UPROPERTY()
	uint8 bMoveBetweenNextOpenedWindow:1;

	UPROPERTY()
	uint8 bAutomaticallyAddToWindow:1;

	UPROPERTY()
	uint8 bItemStacksInteraction:1;
};

/**
 * 
 */
USTRUCT(BlueprintType)
struct FObsidianSlotDefinition
{
	GENERATED_BODY()

public:
	FObsidianSlotDefinition(){};
	FObsidianSlotDefinition(const FGameplayTag& InSlotTag, const FGameplayTagContainer& InAcceptedEquipmentCategories)
		: SlotTag(InSlotTag)
		, AcceptedItemCategories(InAcceptedEquipmentCategories)
	{};

	bool IsValid() const;
	
	FGameplayTag GetSlotTag() const;
	
	EObsidianPlacingAtSlotResult CanPlaceAtSlot(const FGameplayTag& ItemCategory) const;

	void AddBannedItemCategory(const FGameplayTag& InBannedCategory);
	void AddBannedItemCategories(const FGameplayTagContainer& InBannedCategories);
	
	void RemoveBannedItemCategory(const FGameplayTag& BannedCategoryToRemove);
	void RemoveBannedItemCategories(const FGameplayTagContainer& BannedCategoriesToRemove);

public:
	/** Gameplay Tag representing this slot. */
	UPROPERTY(EditDefaultsOnly, Meta = (Categories = "Item.Slot"), Category = "Obsidian")
	FGameplayTag SlotTag = FGameplayTag::EmptyTag;

	/** Items with this Gameplay Tags will be allowed to be equipped in this slot. */
	UPROPERTY(EditDefaultsOnly, Meta = (Categories = "Item.Category"), Category = "Obsidian")
	FGameplayTagContainer AcceptedItemCategories = FGameplayTagContainer::EmptyContainer;

	/** Items with this Gameplay Tags will not be allowed to be equipped in this slot. Can be used to ban some type of armament as a gameplay mechanic. */
	UPROPERTY(EditDefaultsOnly, Meta = (Categories = "Item.Category"), Category = "Obsidian")
	FGameplayTagContainer BannedItemCategories = FGameplayTagContainer::EmptyContainer;
	
	static const FObsidianSlotDefinition InvalidSlot;
};

USTRUCT(BlueprintType)
struct FObsidianItemPosition
{
	GENERATED_BODY()

public:
	FObsidianItemPosition(){}
	FObsidianItemPosition(const FGameplayTag& InSlotTag)
		: SlotTag(InSlotTag)
		, GridLocation(FIntPoint::NoneValue)
		, OwningStashTabTag(FGameplayTag::EmptyTag)
	{}
	FObsidianItemPosition(const FGameplayTag& InSlotTag, const FGameplayTag& InOwningStashTabTag)
		: SlotTag(InSlotTag)
		, GridLocation(FIntPoint::NoneValue)
		, OwningStashTabTag(InOwningStashTabTag)
	{}
	FObsidianItemPosition(const FIntPoint& InGridLocation)
		: SlotTag(FGameplayTag::EmptyTag)
		, GridLocation(InGridLocation)
		, OwningStashTabTag(FGameplayTag::EmptyTag)
	{}
	FObsidianItemPosition(const FIntPoint& InGridLocation, const FGameplayTag& InOwningStashTabTag)
		: SlotTag(FGameplayTag::EmptyTag)
		, GridLocation(InGridLocation)
		, OwningStashTabTag(InOwningStashTabTag)
	{}

	bool IsPositionedOnGrid() const;
	bool IsPositionedAtSlot() const;

	FIntPoint GetItemGridLocation(const bool bWarnIfNotFound = true) const;
	FGameplayTag GetItemSlotTag(const bool bWarnIfNotFound = true) const;
	FGameplayTag GetOwningStashTabTag() const;

	void SetOwningStashTab(const FGameplayTag& InOwningStashTab);
	
	// Type Hash (required for TMap)
	FORCEINLINE friend uint32 GetTypeHash(const FObsidianItemPosition& ItemPosition)
	{
		uint32 Hash = 0;
		Hash = HashCombine(Hash, GetTypeHash(ItemPosition.SlotTag));
		Hash = HashCombine(Hash, GetTypeHash(ItemPosition.GridLocation));
		return Hash;
	}

	// Equality operator (required for TMap)
	FORCEINLINE bool operator==(const FObsidianItemPosition& Other) const
	{
		if (SlotTag.IsValid() && Other.SlotTag.IsValid())
		{
			return SlotTag == Other.SlotTag;
		}
		return GridLocation == Other.GridLocation;
	}

	FString GetDebugStringPosition() const;

private:
	UPROPERTY()
	FGameplayTag SlotTag = FGameplayTag::EmptyTag;

	UPROPERTY()
	FIntPoint GridLocation = FIntPoint::NoneValue;

	UPROPERTY()
	FGameplayTag OwningStashTabTag = FGameplayTag::EmptyTag;
};

/**
 * 
 */
USTRUCT(BlueprintType)
struct FObsidianEquipmentResult
{
	GENERATED_BODY()

public:
	FObsidianEquipmentResult(){};

	FORCEINLINE operator bool() const
	{
		return bActionSuccessful;
	}
	
public:
	/** The action was successful (equipping, de-equipping, replacing with other item). */
	UPROPERTY()
	bool bActionSuccessful = false;

	/** Instance that was equipped or de-equipped as a result of called function, might be nullptr. */
	UPROPERTY()
	TObjectPtr<UObsidianInventoryItemInstance> AffectedInstance = nullptr;
};

/**
 * 
 */
USTRUCT(BlueprintType)
struct FObsidianItemOperationResult
{
	GENERATED_BODY()

public:
	FObsidianItemOperationResult(){};

	FORCEINLINE operator bool() const
	{
		return bActionSuccessful;
	}
	
public:
	/** The action was successful (adding, removing, replacing), the whole item is now in the inventory or removed from. */
	UPROPERTY()
	bool bActionSuccessful = false;

	/** Instance that was relevantly affected by called function, unfortunately this varies through functions, might be nullptr. */
	UPROPERTY()
	TObjectPtr<UObsidianInventoryItemInstance> AffectedInstance = nullptr;

	UPROPERTY()
	int32 StacksLeft = INDEX_NONE;
};

/**
 * 
 */
UENUM(BlueprintType)
enum class EObsidianAddingStacksResultType : uint8
{
	/** The action of adding stacks was unsuccessful. */
	ASR_NoStacksAdded UMETA(DisplayName = "No Stacks Added"),
	
	/** Added at least 1 stack from provided item to some other item. */
	ASR_SomeOfTheStacksAdded UMETA(DisplayName = "Some Of The Stacks Added"),

	/** Whole Item was added as stacks to other Item/Items. */
	ASR_WholeItemAsStacksAdded UMETA(DisplayName = "Whole Item As Stacks Added")
};

/**
 * 
 */
USTRUCT(BlueprintType, meta=(ScriptName = "ObsidianInventoryAddingStacksResult"))
struct FObsidianAddingStacksResult
{
	GENERATED_BODY()

public:
	FObsidianAddingStacksResult(){};
	
public:
	/** The amount of stacks that was added from provided Item to other Item/Items. */
	UPROPERTY(BlueprintReadOnly)
	int32 AddedStacks = INDEX_NONE;

	/** The amount of stacks that is left on the provided Item. */
	UPROPERTY(BlueprintReadOnly)
	int32 StacksLeft = INDEX_NONE;

	/** The result of trying to add stacks. */
	UPROPERTY(BlueprintReadOnly)
	EObsidianAddingStacksResultType AddingStacksResult = EObsidianAddingStacksResultType::ASR_NoStacksAdded;

	/** The last Item Instance that we added some stacks to. */
	UPROPERTY(BlueprintReadOnly)
	UObsidianInventoryItemInstance* LastAddedToInstance = nullptr;
};

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
	PrimaryItemAffix,
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
	FObsidianAffixTier(){};
	
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
	FObsidianAffixIdentifier()
		: bOverride_AttributeToModify(true)
	{}
	
public:
	UPROPERTY(EditDefaultsOnly, Meta = (Categories = "Item.AffixValue"))
	FGameplayTag AffixValueID;

	UPROPERTY(EditAnywhere, meta = (InlineEditConditionToggle), Category = "Obsidian|Affix")
	uint8 bOverride_AttributeToModify : 1;
	
	UPROPERTY(EditDefaultsOnly, Meta = (EditCondition = "bOverride_AttributeToModify"))
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
	bool IsValid() const;
	
public:
	UPROPERTY()
	FObsidianAffixTier AffixTier = FObsidianAffixTier();

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
	FObsidianStaticItemAffix()
	: bOverride_AffixAbilitySet(false)
	{}
	
	bool IsEmptyImplicit() const;
	bool IsEmptyAffix() const;

	explicit operator bool() const;
	bool operator ==(const FObsidianStaticItemAffix& Other) const;
	bool operator ==(const FObsidianDynamicItemAffix& Other) const;
	bool operator ==(const FObsidianActiveItemAffix& Other) const;

#if WITH_EDITOR
	EDataValidationResult IsStaticAffixValid(FDataValidationContext& Context, const int32 Index, const FString& AffixTypeName) const;
#endif

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

	UPROPERTY(EditAnywhere, meta = (InlineEditConditionToggle), Category = "Obsidian|Affix")
	uint8 bOverride_AffixAbilitySet : 1;
	
	/** Soft Ability Set to Apply, usually it will be just Gameplay Effect. */
	UPROPERTY(EditDefaultsOnly, Meta = (EditCondition = "bOverride_AffixAbilitySet"), Category = "Obsidian|Affix")
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
	FObsidianDynamicItemAffix()
		: bOverride_AffixAbilitySet(false)
	{}

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

	UPROPERTY(EditAnywhere, meta = (InlineEditConditionToggle), Category = "Obsidian|Affix")
	uint8 bOverride_AffixAbilitySet : 1;
	
	/** Soft Ability Set to Apply, usually it will be just Gameplay Effect. */
	UPROPERTY(EditDefaultsOnly, Meta = (EditCondition = "bOverride_AffixAbilitySet"), Category = "Obsidian|Affix")
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
	uint8 GetCurrentAffixTierItemLevelRequirement() const;
	
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
struct FObsidianItemGeneratedNameData
{
	GENERATED_BODY()

	FObsidianItemGeneratedNameData(){}
	FObsidianItemGeneratedNameData(const FString& InRareAddition, const FString& InMagicAddition)
		: RareItemDisplayNameAddition(InRareAddition)
		, MagicItemDisplayNameAddition(InMagicAddition)
	{}
public:
	UPROPERTY()
	FString RareItemDisplayNameAddition = FString();

	UPROPERTY()
	FString MagicItemDisplayNameAddition = FString();
};

/**
 * 
 */
USTRUCT(BlueprintType)
struct FObsidianItemGeneratedData
{
	GENERATED_BODY()

public:
	FObsidianItemGeneratedData(){}
	FObsidianItemGeneratedData(const int32 InStacks)
		: AvailableStackCount(InStacks)
	{}
	
	void Reset();
	
public:
	/**
	 *	Stacks data is handled very uniquely. This number represents the Stack Number of the Item that we Templating,
	 * this does not mean that we will add this amount of stacks to the item Instance so we shouldn't initialize the Item
	 * with this amount of stacks.
	 *	Item stacks initialization is handled in Inventory/Stash.
	 */
	UPROPERTY()
	int32 AvailableStackCount = 1;

	UPROPERTY()
	EObsidianItemRarity ItemRarity = EObsidianItemRarity::Normal;

	UPROPERTY()
	TArray<FObsidianActiveItemAffix> ItemAffixes;

	UPROPERTY()
	FObsidianItemGeneratedNameData NameData = FObsidianItemGeneratedNameData();

	UPROPERTY()
	FObsidianItemRequirements ItemEquippingRequirements = FObsidianItemRequirements();
};

/**
 * 
 */
USTRUCT(BlueprintType)
struct FDraggedItem
{
public:
	GENERATED_BODY()
	
	FDraggedItem(){};
	FDraggedItem(UObsidianInventoryItemInstance* InInstance);
	FDraggedItem(const TSubclassOf<UObsidianInventoryItemDefinition>& InItemDef, const FObsidianItemGeneratedData& InGeneratedData)
		: ItemDef(InItemDef)
		, GeneratedData(InGeneratedData)
	{}

	bool IsEmpty() const;
	bool CarriesItemDef() const;

	bool CarriesInstance() const
	{
		return Instance != nullptr;
	}

	void Clear()
	{
		Instance = nullptr;
		ItemDef = nullptr;
		GeneratedData.Reset();
	}

public:
	UPROPERTY()
	TObjectPtr<UObsidianInventoryItemInstance> Instance = nullptr;

	UPROPERTY()
	TSubclassOf<UObsidianInventoryItemDefinition> ItemDef = nullptr;
	
	UPROPERTY()
	FObsidianItemGeneratedData GeneratedData = FObsidianItemGeneratedData();
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
struct FObsidianItemRequirementsUIDescription
{
	GENERATED_BODY()

	FObsidianItemRequirementsUIDescription()
		: bHasHeroClassRequirement(false)
		, bHasLevelRequirement(false)
		, bHasStrengthRequirement(false)
		, bHasDexterityRequirement(false)
		, bHasFaithRequirement(false)
		, bHasIntelligenceRequirement(false)
		, bMeetHeroClassRequirement(false)
		, bMeetLevelRequirement(false)
		, bMeetStrengthRequirement(false)
		, bMeetDexterityRequirement(false)
		, bMeetFaithRequirement(false)
		, bMeetIntelligenceRequirement(false)
	{}

	void SetHeroLevelRequirement(const uint8 RequiredMagnitude, const uint8 OwnerMagnitude);
	void SetHeroClassRequirement(const EObsidianHeroClass RequiredClass, const EObsidianHeroClass OwnerClass);
	void SetAttributeRequirement(const FGameplayAttribute& Attribute, const float RequirementMagnitude, const float OwnerMagnitude);
	
public:
	uint8 bHasHeroClassRequirement:1;
	uint8 bHasLevelRequirement:1;
	uint8 bHasStrengthRequirement:1;
	uint8 bHasDexterityRequirement:1;
	uint8 bHasFaithRequirement:1;
	uint8 bHasIntelligenceRequirement:1;
	
	uint8 bMeetHeroClassRequirement:1;
	uint8 bMeetLevelRequirement:1;
	uint8 bMeetStrengthRequirement:1;
	uint8 bMeetDexterityRequirement:1;
	uint8 bMeetFaithRequirement:1;
	uint8 bMeetIntelligenceRequirement:1;
	
	UPROPERTY()
	FText HeroClassRequirementText;

	UPROPERTY()
	uint8 LevelRequirement = 0;

	UPROPERTY()
	uint16 StrengthRequirement = 0;
	UPROPERTY()
	uint16 DexterityRequirement = 0;
	UPROPERTY()
	uint16 FaithRequirement = 0;
	UPROPERTY()
	uint16 IntelligenceRequirement = 0;
};

USTRUCT(BlueprintType)
struct FObsidianItemStats
{
	GENERATED_BODY()

public:
	FObsidianItemStats()
		: bContainsItemImage(false)
		, bContainsDisplayName(false)
		, bContainsRareItemDisplayNameAddition(false)
		, bContainsMagicItemDisplayNameAddition(false)
		, bContainsDescription(false)
		, bContainsAdditionalDescription(false)
		, bContainsStacks(false)
		, bContainsAffixes(false)
		, bSupportIdentification(false)
		, bHasItemEquippingRequirements(false)
	{}

	// ----------------------- Containers hehe (checks) ----------------------- 
	
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

	/** Checks if the Item has equipping requirements. */
	bool HasItemEquippingRequirements() const
	{
		return bHasItemEquippingRequirements;
	}

	// ----------------------- Getters ----------------------- 

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

	FObsidianItemRequirementsUIDescription GetItemEquippingRequirements() const
	{
		return ItemEquippingRequirements;
	}
	

	// ----------------------- Setters ----------------------- 

	/** It takes an additional ItemGridSpan argument as UI needs it to rescale the image for now. */
	void SetItemImage(UTexture2D* InItemImage, const FIntPoint& InItemGridSpan);
	void SetDisplayName(const FText& InDisplayName);
	void SetRareDisplayNameAddition(const FString& InDisplayNameAddition);
	void SetMagicDisplayNameAddition(const FString& InDisplayNameAddition);
	void SetDescription(const FText& InDescription);
	void SetAdditionalDescription(const FText& InAdditionalDescription);
	void SetStacks(const int32 InCurrentStack, const int32 InMaxStacks);
	void SetCurrentStacks(const int32 InCurrentStack);
	void SetMaxStacks(const int32 InMaxStacks);
	void SetIdentified(const bool InIdentified);
	void SetAffixDescriptionRows(const TArray<FObsidianAffixDescriptionRow>& AffixRows);
	void SetItemEquippingRequirements(const FObsidianItemRequirementsUIDescription& Requirements);
	
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

	UPROPERTY()
	bool bIdentified = false;

	/**
	 * Requirements.
	 */

	UPROPERTY()
	FObsidianItemRequirementsUIDescription ItemEquippingRequirements = FObsidianItemRequirementsUIDescription();
	
	/**
	 * Contains booleans.
	 */
	
	uint8 bContainsItemImage:1;
	uint8 bContainsDisplayName:1;
	uint8 bContainsRareItemDisplayNameAddition:1;
	uint8 bContainsMagicItemDisplayNameAddition:1;
	uint8 bContainsDescription:1;
	uint8 bContainsAdditionalDescription:1;
	uint8 bContainsStacks:1;
	uint8 bContainsAffixes:1;
	uint8 bSupportIdentification:1;
	uint8 bHasItemEquippingRequirements:1;
};



