// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"

// ~ Project

#include "InventoryItems/ItemAffixes/ObsidianAffixList.h"
#include "ObsidianTreasureList.generated.h"

class UObsidianInventoryItemDefinition;

USTRUCT()
struct FObsidianStacksToDrop
{
	GENERATED_BODY()
	
public:
	FObsidianStacksToDrop(){}
	FObsidianStacksToDrop(const uint8 InStackSize, const uint16 InDropWeight)
		: StackSize(InStackSize)
		, DropWeight(InDropWeight)
	{}
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	uint8 StackSize = 1;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian", meta=(ClampMin = "0", ClampMax = "1000"))
	uint16 DropWeight = 1000; 
};

USTRUCT()
struct FObsidianDropItem
{
	GENERATED_BODY()

public:
	FObsidianDropItem();

	bool IsValid() const;

	uint8 GetRandomStackSizeToDropAdjusted(const uint8 TreasureQuality) const;
	
public:
	/** Actual items/item templates in this Treasure Class. */
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	TSoftClassPtr<UObsidianInventoryItemDefinition> SoftTreasureItemDefinitionClass = nullptr;

	/** Drop Weight [0, 1000], the higher the weight the more likely the item to drop. */
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian", meta=(ClampMin = "0", ClampMax = "1000"))
	uint16 DropWeight = 1;
	
	/** Array of possible weighted stack sizes to drop the Item with. */
	UPROPERTY(EditDefaultsOnly, Meta = (EditCondition = "bStackable"), Category = "Obsidian")
	TArray<FObsidianStacksToDrop> StackSizes;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	bool bShouldRandomizeRarity = true;
	
	UPROPERTY(EditDefaultsOnly, Meta = (EditCondition = "bShouldRandomizeRarity", EditConditionHides), Category = "Obsidian")
	EObsidianItemRarity ItemMaxRarity = EObsidianItemRarity::Set;
	
	/** Derived automatically from provided Treasure Item Definition. */
	UPROPERTY(VisibleDefaultsOnly, Category = "Obsidian")
	FGameplayTag ItemBaseType = FGameplayTag::EmptyTag;
	
	static const FObsidianDropItem NoDropType;

protected:
	//TODO(intrxx) Validation of already added items to check if they are actually stackable?
	/** Flag used for editing the StackSizes Array as not every item is stackable and therefore cares about it. */
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	bool bStackable = false;
};

/**
 * Dynamic Item Definition of Item to drop.
 */
USTRUCT()
struct FObsidianItemToDrop
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TSubclassOf<UObsidianInventoryItemDefinition> ItemDefinitionClass;

	UPROPERTY()
	TArray<FObsidianActiveItemAffix> DropAffixes;
	
	UPROPERTY()
	FTransform DropTransform = FTransform::Identity;

	UPROPERTY()
	EObsidianItemRarity DropRarity = EObsidianItemRarity::Normal;
	
	UPROPERTY()
	uint8 DropStacks = 1;
};

USTRUCT()
struct FObsidianTreasureClass
{
	GENERATED_BODY()

public:
	FObsidianTreasureClass(){}
	FObsidianTreasureClass(const TArray<FObsidianDropItem>& InDropItems)
		: DropItems(InDropItems)
	{}
	
	/** Returns Weighted Randomized Item, will be nullptr if NoDrop was chosen. */
	FObsidianDropItem GetRandomItemFromClass(const float NoDropScale = 1.0f);

#if WITH_EDITOR
	EDataValidationResult ValidateData(FDataValidationContext& Context, const int Index) const;
#endif
	
public:
	//TODO(intrxx) Currently not used for anything, keep or delete?
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	FName TreasureClassName;
	
	/**
	 * Treasure Quality [0, 85] is an identifier of value of items listed in this Treasure Class.
	 * Once the enemy is killed, the area level is gathered then the rarity of monster gets added to the number,
	 * [Normal = 0], [Elite = 3], [Boss = 4], [Special Boss = 5]. Once the number is evaluated, all Treasure Classes up
	 * to this number are gathered and rolling for item begins.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian", meta=(ClampMin = "0", ClampMax = "85"))
	uint8 TreasureQuality = 0;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian", meta=(ClampMin = "0"))
	uint16 NoDropWeight = 0;

	/** Definition of Drop Item. */
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	TArray<FObsidianDropItem> DropItems;
};

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianTreasureList : public UDataAsset
{
	GENERATED_BODY()

public:
	virtual void PostInitProperties() override;
	virtual void PostLoad() override;
	
	TArray<FObsidianTreasureClass> GetAllTreasureClasses() const;
	TArray<FObsidianTreasureClass> GetAllTreasureClassesUpToQuality(const uint8 TreasureQuality);
	TArray<FObsidianTreasureClass> GetTreasureClassesOfQuality(const uint8 TreasureQuality) const;
	TArray<FObsidianDropItem> GetAllItemsOfBaseTypeUpToQuality(const uint8 TreasureQuality, const FGameplayTag& OfBaseType);
	
	virtual void PreSave(FObjectPreSaveContext SaveContext) override;
	
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	TArray<FObsidianTreasureClass> TreasureClasses;
	
	/** MultiMap for faster lookups. */
	TMultiMap<int32, const FObsidianTreasureClass*> TreasureClassMap;
};
