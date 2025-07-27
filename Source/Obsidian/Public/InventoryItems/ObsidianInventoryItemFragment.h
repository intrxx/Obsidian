// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project


#include "ObsidianInventoryItemFragment.generated.h"

class UObsidianInventoryItemDefinition;
class UObsidianInventoryItemInstance;

/**
 * Represents a fragment of an item definition, can be treated like a component to an item.
 */
UCLASS(DefaultToInstanced, EditInlineNew, Abstract)
class OBSIDIAN_API UObsidianInventoryItemFragment : public UObject
{
	GENERATED_BODY()

public:
	virtual void OnInstancedCreated(UObsidianInventoryItemInstance* Instance) const {}
};

