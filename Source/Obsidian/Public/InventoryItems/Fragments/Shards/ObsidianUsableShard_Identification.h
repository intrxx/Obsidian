// Copyright 2026 out of sCope team - intrxx

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project


#include "InventoryItems/Fragments/Shards/ObsidianUsableShard.h"
#include "ObsidianUsableShard_Identification.generated.h"

/**
 * Identifies an item.
 */
UCLASS(DisplayName="Identification")
class OBSIDIAN_API UObsidianUsableShard_Identification : public UObsidianUsableShard
{
	GENERATED_BODY()

public:
	virtual bool OnItemUsed(AObsidianPlayerController* ItemOwner, UObsidianInventoryItemInstance* UsingInstance, UObsidianInventoryItemInstance* UsingOntoInstance = nullptr) override;
	virtual FObsidianItemsMatchingUsableContext OnItemUsed_UIContext(const TArray<UObsidianInventoryItemInstance*>& AllItems) override;
};
