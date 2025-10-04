// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project
#include "Obsidian/ObsidianGameplayTags.h"

#include "InventoryItems/ObsidianInventoryItemDefinition.h"
#include "ObsidianItemTypes.generated.h"

struct FObsidianDynamicItemAffix;

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
namespace ObsidianTreasureStatics
{
	extern const uint8 MaxTreasureQuality;
	extern const TMap<EObsidianEntityRarity, uint8> DefaultRarityToNumberOfDropRollsMap;
	extern const TMap<EObsidianEntityRarity, uint8> DefaultRarityToAddedTreasureQualityMap;
	
	extern const TMap<FGameplayTag, uint16> DefaultRarityToWeightMap;
	extern const TMap<FGameplayTag, uint8> DefaultRarityToMaxSuffixCount;
	extern const TMap<FGameplayTag, uint8> DefaultRarityToMaxPrefixCount;
	extern const TMap<FGameplayTag, uint8> DefaultRarityToMaxAffixCount;
	extern const uint8 DefaultMaxImplicitCount;

	inline uint8 GetMaxPrefixCountForRarity(const FGameplayTag& RarityTag)
	{
		if (const uint8* CountPtr = DefaultRarityToMaxPrefixCount.Find(RarityTag))
		{
			return *CountPtr;
		}
		return 0;
	}
	
	inline uint8 GetMaxSuffixCountForRarity(const FGameplayTag& RarityTag)
	{
		if (const uint8* CountPtr = DefaultRarityToMaxSuffixCount.Find(RarityTag))
		{
			return *CountPtr;
		}
		return 0;
	}
	
	inline uint8 GetMaxAffixCountForRarity(const FGameplayTag& RarityTag)
	{
		if (const uint8* CountPtr = DefaultRarityToMaxAffixCount.Find(RarityTag))
		{
			return *CountPtr;
		}
		return 0;
	}
}

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
 * 
 */
namespace ObsidianAffixLimit
{
	static constexpr int32 NormalItemAffixesCountLimit = 0;
	static constexpr int32 MagicItemAffixesCountLimit = 2;
	static constexpr int32 RareItemAffixesCountLimit = 6;

	/** Returns the unified project default item affix limit for a given Stack Tag. */
	inline int32 GetUnifiedDefaultForRarityTag(const FGameplayTag Tag)
	{
		if(Tag == ObsidianGameplayTags::Item_Rarity_Normal)
		{
			return NormalItemAffixesCountLimit;
		}
		if(Tag == ObsidianGameplayTags::Item_Rarity_Magic)
		{
			return MagicItemAffixesCountLimit;
		}
		if(Tag == ObsidianGameplayTags::Item_Rarity_Rare)
		{
			return RareItemAffixesCountLimit;
		}

		UE_LOG(LogTemp, Error, TEXT("Provided Item Rarity Gameplay Tag is invalid, returning 0."));
		return 0;
	}
}

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
	ItemUnfitForCategory UMETA(DisplayName="Item Unfit For Category"),
	
	/** Item cannot be equipped, it does not have equipment fragment. */
	ItemUnequippable UMETA(DisplayName="Item Unequippable"),

	/** Player cannot modify the state of the equipment. */
	EquipmentActionsBlocked UMETA(DisplayName="Equipment Actions Blocked"),

	/** Item cannot be equipped, trying to equip two-handed weapon, two slots are occupied and no sufficient space in inventory. */
	UnableToEquip_NoSufficientInventorySpace UMETA(DisplayName="Unable To Equip - No Sufficient Inventory Space"),

	/** Item cannot be equipped, category is banned by gameplay mechanic. */
	UnableToEquip_BannedCategory UMETA(DisplayName="Unable To Equip - Banned Category"),
	
	/** Item cannot be equipped, item cannot be equipped together with the item in sister slot. */
	UnableToEquip_DoesNotFitWithOtherWeaponType UMETA(DisplayName="Unable To Equip - Item Does not fit with other weapon type."),
	
	/** Item cannot be equipped, it needs to be identified first. */
	ItemUnientified UMETA(DisplayName="Item Unientified"),

	/** Item cannot be equipped, no sufficient hero level. */
	NotEnoughHeroLevel UMETA(DisplayName="Not Enough Hero Level"),

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
	bool HasLimitedStacks() const;

	FGameplayTag GetSlotTag() const;
	
	EObsidianEquipCheckResult CanPlaceAtSlot(const FGameplayTag& ItemCategory) const;

	void AddBannedItemCategory(const FGameplayTag& InBannedCategory);
	void AddBannedItemCategories(const FGameplayTagContainer& InBannedCategories);
	
	void RemoveBannedItemCategory(const FGameplayTag& BannedCategoryToRemove);
	void RemoveBannedItemCategories(const FGameplayTagContainer& BannedCategoriesToRemove);

public:
	/** Gameplay Tag representing this slot. */
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	FGameplayTag SlotTag = FGameplayTag::EmptyTag;

	/** Items with this Gameplay Tags will be allowed to be equipped in this slot. */
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	FGameplayTagContainer AcceptedItemCategories = FGameplayTagContainer::EmptyContainer;

	/** Items with this Gameplay Tags will not be allowed to be equipped in this slot. Can be used to ban some type of armament as a gameplay mechanic. */
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	FGameplayTagContainer BannedItemCategories = FGameplayTagContainer::EmptyContainer;

	/** - UNIMPLEMENTED - Amount of Stacks of provided Item the Slot can store. Leave at INDEX_NONE for unlimited storage, this is the default behaviour for Equipment Slots and shouldn't be changed. */
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	int32 SlotStackLimit = INDEX_NONE;

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
struct FObsidianItemGeneratedData
{
	GENERATED_BODY()

public:
	FObsidianItemGeneratedData(){}
	FObsidianItemGeneratedData(const int32 InStacks)
		: StackCount(InStacks)
	{}
	FObsidianItemGeneratedData(const int32 InStacks, const FGameplayTag& InRarityTag, const TArray<FObsidianActiveItemAffix>& InAffixes)
		: StackCount(InStacks)
		, ItemRarityTag(InRarityTag)
		, ItemAffixes(InAffixes)
	{}

	void Reset();
	
public:
	UPROPERTY()
	int32 StackCount = 1;

	UPROPERTY()
	FGameplayTag ItemRarityTag = ObsidianGameplayTags::Item_Rarity_Normal;

	UPROPERTY()
	TArray<FObsidianActiveItemAffix> ItemAffixes;
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

	bool IsEmpty() const
	{
		return !Instance && !ItemDef;
	}

	bool CarriesItemDef() const
	{
		return ItemDef != nullptr;
	}

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
	FObsidianItemGeneratedData GeneratedData = 0;
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


