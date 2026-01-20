// Copyright 2026 out of sCope team - intrxx

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project


#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "InventoryItems/Fragments/Shards/ObsidianUsableShard.h"
#include "ObsidianUsableShard_OrbOfEnchantment.generated.h"

/**
 * 
 */
UCLASS(DisplayName="Orb of Enchantment")
class OBSIDIAN_API UObsidianUsableShard_OrbOfEnchantment : public UObsidianUsableShard
{
	GENERATED_BODY()

public:
	virtual bool OnItemUsed(AObsidianPlayerController* ItemOwner, UObsidianInventoryItemInstance* UsingInstance, UObsidianInventoryItemInstance* UsingOntoInstance = nullptr) override;
	virtual FObsidianItemsMatchingUsableContext OnItemUsed_UIContext(const TArray<UObsidianInventoryItemInstance*>& AllItems) override;

protected:
	bool CanUseOnItem(const UObsidianInventoryItemInstance* Instance) const;
};
