// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"

#include "InventoryItems/Fragments/Shards/ObsidianUsableShard.h"

#include "ObsidianUsableShard_OrbOfEnchantment.generated.h"

class UObsidianInventoryItemInstance;

/**
 * Orb Of Enchantment adds Affix or Suffix to item if possible.
 */
UCLASS(DisplayName = "Orb of Enchantment")
class OBSIDIAN_API UObsidianUsableShard_OrbOfEnchantment : public UObsidianUsableShard
{
	GENERATED_BODY()

public:
	virtual bool OnItemUsed(AObsidianPlayerController* InItemOwner, UObsidianInventoryItemInstance* InUsingInstance,
		UObsidianInventoryItemInstance* InUsingOntoInstance = nullptr) override;
	virtual void OnItemUsed_UIContext(const TArray<UObsidianInventoryItemInstance*>& InAllItems,
		FObsidianItemsMatchingUsableContext& OutItemsMatchingContext) override;

protected:
	bool CanUseOnItem(const UObsidianInventoryItemInstance* InInstance) const;
};
