// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

// ~ Project
#include "ObsidianTypes/ItemTypes/ObsidianItemTypes.h"

#include "InventoryItems/ObsidianInventoryItemFragment.h"
#include "OInventoryItemFragment_Affixes.generated.h"

/**
 * The depth of Item Affix generation.
 * List of possible Affixes to generate:
 * - Suffixes
 * - Prefixes
 * - Implicit
 */
UENUM(BlueprintType)
enum class EObsidianAffixGenerationType : uint8
{
	/** No Affix will be generated upon dropping the item. You can specify Affixes in the Definition. */
	NoGeneration = 0,

	/** Default path, only Prefixes and Suffixes will be generated. You can specify Implicit in the Definition. */
	DefaultGeneration,

	/** Prefixes, Suffixes and Implicit will be generated. You cannot specify anything. */
	FullGeneration
};

/**
 * 
 */
UCLASS(DisplayName = "Affixes")
class OBSIDIAN_API UOInventoryItemFragment_Affixes : public UObsidianInventoryItemFragment
{
	GENERATED_BODY()

public:
    UOInventoryItemFragment_Affixes(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
    
	//~ Start of UObsidianInventoryItemFragment
	virtual void OnInstancedCreated(UObsidianInventoryItemInstance* Instance) const override;
	//~ End of UObsidianInventoryItemFragment
	
	bool HasImplicitAffix() const;
	
	/** Returns true if the item is either Unique or Set, fills the Rarity Tag if thats the case. */
	bool IsUniqueOrSet_GetRarity(FGameplayTag& OutRarity) const;

	FObsidianStaticItemAffix GetStaticImplicitAffix() const;
	TArray<FObsidianStaticItemAffix> GetStaticAffixes() const;
	EObsidianAffixGenerationType GetGenerationType() const;

protected:
	//TODO(intrxx) bind to changing property, set ItemRarityTag, ItemAffixes and bStartsIdentified to default when switching this back to true
	/** Whether this item will be generated at drop, uncheck if you want to create a static item template that will be dropped as is (e.g. for unique items). */
	UPROPERTY(EditDefaultsOnly, Category = "Affixes")
	EObsidianAffixGenerationType ItemAffixesGenerationType = EObsidianAffixGenerationType::DefaultGeneration;
	
	/** Item Rarity Tag, although it is exposed it should only be used to design Unique Items. */
	UPROPERTY(EditDefaultsOnly, meta=(Categories = "Item.Rarity", EditCondition = "ItemAffixesGenerationType==EObsidianAffixGenerationType::NoGeneration"),  Category = "Affixes")
	FGameplayTag ItemRarityTag = ObsidianGameplayTags::Item_Rarity_Normal;

	UPROPERTY(EditDefaultsOnly, Meta = (EditCondition = "ItemAffixesGenerationType!=EObsidianAffixGenerationType::FullGeneration"), Category = "Affixes")
	bool bHasImplicitAffix = false;
	
	UPROPERTY(EditDefaultsOnly, Meta=(EditCondition = "ItemAffixesGenerationType!=EObsidianAffixGenerationType::FullGeneration&&bHasImplicitAffix"), Category = "Affixes")
	FObsidianStaticItemAffix StaticItemImplicit;
	
	UPROPERTY(EditDefaultsOnly, Meta=(EditCondition = "ItemAffixesGenerationType==EObsidianAffixGenerationType::NoGeneration"), Category = "Affixes")
	TArray<FObsidianStaticItemAffix> StaticItemAffixes;
};
