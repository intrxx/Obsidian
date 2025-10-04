// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project
#include "Obsidian/ObsidianGameplayTags.h"
#include "ObsidianItemAffixTypes.h"
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



