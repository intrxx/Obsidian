// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "ObsidianUsableShard.generated.h"

class UObsidianInventoryItemInstance;

/**
 * Represent some OnItemUse logic for an Item.
 */
UCLASS(DefaultToInstanced, EditInlineNew, Abstract)
class OBSIDIAN_API UObsidianUsableShard : public UObject
{
	GENERATED_BODY()
	
public:
	virtual void OnItemUsed(UObsidianInventoryItemInstance* UsingInstance, UObsidianInventoryItemInstance* UsingOntoInstance) {}
};
