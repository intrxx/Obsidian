// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "InventoryItems/ObsidianInventoryItemFragment.h"
#include "Obsidian/ObsidianGameplayTags.h"
#include "ObsidianTypes/ObsidianItemTypes.h"
#include "OInventoryItemFragment_Affixes.generated.h"

USTRUCT(BlueprintType)
struct FObsidianItemAffix
{
	GENERATED_BODY()

public:
	FObsidianItemAffix(){}

	explicit operator bool() const
	{
		return AffixTag.IsValid();
	}
	
public:
	UPROPERTY(EditDefaultsOnly, meta=(Categories = "Item.Affix"))
	FGameplayTag AffixTag = FGameplayTag::EmptyTag;

	UPROPERTY(EditDefaultsOnly)
	EObsidianAffixType AffixType = EObsidianAffixType::None;

	UPROPERTY(EditDefaultsOnly)
	int32 AffixTier = -1;
	
	UPROPERTY(EditDefaultsOnly)
	FText AffixDescription = FText();

	/**TODO just a simple int for now, will cover more later. */
	UPROPERTY(EditDefaultsOnly)
	int32 TempAffixMagnitude = 0;
};

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

	bool IsItemIdentified() const;

	FGameplayTag GetItemRarityTag() const
	{
		return ItemRarityTag;
	}

	UFUNCTION(BlueprintCallable, Category = "Obsidian|Inventory")
	TArray<FObsidianAffixDescriptionRow> GetAffixesAsUIDescription() const;

protected:
	/** Item Rarity Tag, although it is exposed it should only be used to design Unique Items. */
	UPROPERTY(EditDefaultsOnly, meta=(Categories = "Item.Rarity"), Category = "Affixes")
	FGameplayTag ItemRarityTag = ObsidianGameplayTags::Item_Rarity_Normal;

	UPROPERTY(EditDefaultsOnly, Category = "Affixes")
	TArray<FObsidianItemAffix> ItemAffixes;
	
	/** Whether or not the item starts identified, normal, items without any affixes or items with this bool set to true will start as identified. */
	UPROPERTY(EditDefaultsOnly, Category = "Affixes")
	bool bStartsIdentified = false;

private:
	int32 AddedSuffixCount = 0;
	int32 AddedPrefixCount = 0;
};
