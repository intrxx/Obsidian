// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

// ~ Project
#include "ObsidianTypes/ObsidianItemTypes.h"

#include "InventoryItems/ObsidianInventoryItemFragment.h"
#include "OInventoryItemFragment_Affixes.generated.h"

/**
 * 
 */
UCLASS(DisplayName="Affixes")
class OBSIDIAN_API UOInventoryItemFragment_Affixes : public UObsidianInventoryItemFragment
{
	GENERATED_BODY()

public:
	//~ Start of UObsidianInventoryItemFragment
	virtual void OnInstancedCreated(UObsidianInventoryItemInstance* Instance) const override;
	//~ End of UObsidianInventoryItemFragment
	
	void AddItemAffixes(const TArray<FObsidianRandomItemAffix>& InItemAffixes, const FGameplayTag& InRarityTag);

	bool IsItemIdentified() const;

	FGameplayTag GetItemRarityTag() const
	{
		return ItemRarityTag;
	}

	bool ShouldBeGeneratedAtDrop() const;

	UFUNCTION(BlueprintCallable, Category = "Obsidian|Inventory")
	TArray<FObsidianAffixDescriptionRow> GetAffixesAsUIDescription() const;

protected:
	//TODO(intrxx) bind to changing property, set ItemRarityTag, ItemAffixes and bStartsIdentified to default when switching this back to true
	/** Whether this item will be generated at drop, uncheck if you want to create a static item template that will be dropped as is (e.g. for unique items). */
	UPROPERTY(EditDefaultsOnly, Category = "Affixes")
	bool bGeneratedAtDrop = true;
	
	/** Item Rarity Tag, although it is exposed it should only be used to design Unique Items. */
	UPROPERTY(EditDefaultsOnly, meta=(Categories = "Item.Rarity", EditCondition = "bGeneratedAtDrop==false"),  Category = "Affixes")
	FGameplayTag ItemRarityTag = FGameplayTag::EmptyTag;

	UPROPERTY(EditDefaultsOnly, Meta=(EditCondition = "bGeneratedAtDrop==false"), Category = "Affixes")
	TArray<FObsidianRandomItemAffix> ItemAffixes;
	
	/** Whether or not the item starts identified, normal, items without any affixes or items with this bool set to true will start as identified. */
	UPROPERTY(EditDefaultsOnly, Meta=(EditCondition = "bGeneratedAtDrop==false"), Category = "Affixes")
	bool bStartsIdentified = false;

private:
	int32 AddedSuffixCount = 0;
	int32 AddedPrefixCount = 0;
};
