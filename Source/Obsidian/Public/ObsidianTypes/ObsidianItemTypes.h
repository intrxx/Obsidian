// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project
#include "Obsidian/ObsidianGameplayTags.h"

#include "InventoryItems/ObsidianInventoryItemDefinition.h"
#include "ObsidianItemTypes.generated.h"

class UObsidianInventoryItemInstance;

/**
 * 
 */
namespace ObsidianInventoryItemsStatics
{
	static const FVector2D InventorySlotSize(64.0f, 64.0f);

	extern const TMap<FGameplayTag, FName> SlotToAttachSocketMap;

	/** Accepted Equipment Categories for sister slot per weapon type. */
	extern const TMap<FGameplayTag, FGameplayTagContainer> AcceptedSisterSlotEquipmentCategoriesPerEquipmentCategory;
	
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

/**
 * 
 */
UENUM(BlueprintType)
enum class EObsidianEquipResult : uint8
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

/**
 * 
 */
UENUM(BlueprintType)
enum class EObsidianAffixType : uint8
{
	None = 0,
	Implicit,
	Prefix,
	Suffix,
};

/**
 * 
 */
USTRUCT(BlueprintType)
struct FObsidianAddingStacksResult
{
	GENERATED_BODY()

public:
	/** The amount of stacks that was added from provided Item to other Item/Items. */
	UPROPERTY(BlueprintReadOnly)
	int32 AddedStacks = 0;

	/** The amount of stacks that is left on the provided Item. */
	UPROPERTY(BlueprintReadOnly)
	int32 StacksLeft = -1;
	
	/** Whole Item was added as stacks to other Item/Items. */
	UPROPERTY(BlueprintReadOnly)
	bool bAddedWholeItemAsStacks = false;

	/** Added at least 1 stack from provided item to some other item. */
	UPROPERTY(BlueprintReadOnly)
	bool bAddedSomeOfTheStacks = false;

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
USTRUCT()
struct FDraggedItem
{
public:
	GENERATED_BODY()
	
	FDraggedItem(){};
	FDraggedItem(UObsidianInventoryItemInstance* InInstance);
	FDraggedItem(const TSubclassOf<UObsidianInventoryItemDefinition>& InItemDef, const int32 InStacks)
		: ItemDef(InItemDef)
		, Stacks(InStacks)
	{}

	bool IsEmpty() const
	{
		return !Instance && !ItemDef;
	}

	void Clear()
	{
		Instance = nullptr;
		ItemDef = nullptr;
		Stacks = 0;
	}

public:
	UPROPERTY()
	TObjectPtr<UObsidianInventoryItemInstance> Instance = nullptr;

	UPROPERTY()
	TSubclassOf<UObsidianInventoryItemDefinition> ItemDef = nullptr;
	
	UPROPERTY()
	int32 Stacks = 0;
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


