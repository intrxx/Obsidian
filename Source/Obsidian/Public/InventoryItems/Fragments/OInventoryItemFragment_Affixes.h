// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "InventoryItems/ObsidianInventoryItemFragment.h"
#include "Obsidian/ObsidianGameplayTags.h"
#include "OInventoryItemFragment_Affixes.generated.h"

UENUM(BlueprintType)
enum class EObsidianAffixType : uint8
{
	None = 0,
	Implicit,
	Prefix,
	Suffix,
};

USTRUCT(BlueprintType)
struct FObsidianItemAffix
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FText StatDescription = FText();

	UPROPERTY()
	EObsidianAffixType AffixType = EObsidianAffixType::None;

	int32 AffixTier = -1;
};

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UOInventoryItemFragment_Affixes : public UObsidianInventoryItemFragment
{
	GENERATED_BODY()

public:
	//~ Start of UObsidianInventoryItemFragment
	virtual void OnInstancedCreated(UObsidianInventoryItemInstance* Instance) const override;
	//~ End of UObsidianInventoryItemFragment

protected:
	/** Item Rarity Tag, although it is exposed it should only be used to design Unique Items. */
	UPROPERTY(EditDefaultsOnly, Category = "Affixes")
	FGameplayTag ItemRarityTag = ObsidianGameplayTags::Item_Rarity_Normal;

	UPROPERTY(EditDefaultsOnly, Category = "Affixes")
	TArray<FObsidianItemAffix> ItemAffixes;
	
private:
	int32 AddedSuffixCount = 0;
	int32 AddedPrefixCount = 0;
};
