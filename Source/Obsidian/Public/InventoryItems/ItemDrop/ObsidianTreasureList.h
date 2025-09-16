// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project


#include "Engine/DataAsset.h"
#include "ObsidianTreasureList.generated.h"

class UObsidianInventoryItemDefinition;

USTRUCT()
struct FObsidianDropItem
{
	GENERATED_BODY()
	
	/** Actual items/item templates in this Treasure Class. */
	UPROPERTY(EditAnywhere, Category = "Obsidian")
	TSubclassOf<UObsidianInventoryItemDefinition> TreasureItemDefinitionClasses;

	/** Drop Weight [0, 1000], the higher the weight the more likely the item to drop. */
	UPROPERTY(EditAnywhere, Category = "Obsidian", meta=(ClampMin = "0", ClampMax = "1000"))
	int32 DropWeight = 1; 
};

USTRUCT()
struct FObsidianTreasureClass
{
	GENERATED_BODY()

	/**
	 * Treasure Quality [0, 90] is an identifier of value of items listed in this Treasure Class.
	 * Once the enemy is killed, the area level is gathered then the rarity of monster gets added to the number,
	 * [Normal = 0], [Elite = 3], [Boss = 4], [Special Boss = 5]. Once the number is evaluated, all Treasure Classes up
	 * to this number are gathered and rolling for item begins.
	 */
	UPROPERTY(EditAnywhere, Category = "Obsidian", meta=(ClampMin = "0", ClampMax = "85"))
	int32 TreasureQuality = 0;

	/** Definition of Drop Item. */
	UPROPERTY(EditAnywhere, Category = "Obsidian")
	TArray<FObsidianDropItem> DropItems;

#if WITH_EDITOR
	EDataValidationResult ValidateData(FDataValidationContext& Context, const int Index) const;
#endif
};

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianTreasureList : public UDataAsset
{
	GENERATED_BODY()

public:
	virtual void PostLoad() override;

	TArray<FObsidianTreasureClass> GetAllTreasureClassesUpToQuality(const int32 TreasureQuality);
	const FObsidianTreasureClass* GetTreasureClassOfQuality(const int32 TreasureQuality);

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	TArray<FObsidianTreasureClass> TreasureClasses;

	/** Map for faster lookups. */
	TMap<int32, const FObsidianTreasureClass*> TreasureClassMap;
};
