// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project


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
};
