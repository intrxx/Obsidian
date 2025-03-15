// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
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
	virtual void OnItemUsed(UObsidianInventoryItemInstance* UsingInstance, UObsidianInventoryItemInstance* UsingOntoInstance) override;
	
};
