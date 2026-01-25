// Copyright 2026 out of sCope team - intrxx

#pragma once

#include <CoreMinimal.h>


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
	virtual bool OnItemUsed(AObsidianPlayerController* ItemOwner, UObsidianInventoryItemInstance* UsingInstance,
		UObsidianInventoryItemInstance* UsingOntoInstance = nullptr) override;
	virtual void OnItemUsed_UIContext(const TArray<UObsidianInventoryItemInstance*>& AllItems,
		FObsidianItemsMatchingUsableContext& OutItemsMatchingContext) override;
};
