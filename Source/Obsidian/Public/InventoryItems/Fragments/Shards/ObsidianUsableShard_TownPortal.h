// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project


#include "InventoryItems/Fragments/Shards/ObsidianUsableShard.h"
#include "ObsidianUsableShard_TownPortal.generated.h"

/**
 * 
 */
UCLASS(DisplayName="Town Portal")
class OBSIDIAN_API UObsidianUsableShard_TownPortal : public UObsidianUsableShard
{
	GENERATED_BODY()

public:
	virtual bool OnItemUsed(AObsidianPlayerController* ItemOwner, UObsidianInventoryItemInstance* UsingInstance, UObsidianInventoryItemInstance* UsingOntoInstance = nullptr) override; 
};
