// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "InventoryItems/ObsidianInventoryItemFragment.h"
#include "OInventoryItemFragment_Usable.generated.h"

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UOInventoryItemFragment_Usable : public UObsidianInventoryItemFragment
{
	GENERATED_BODY()
	
public:
	//~ Start of UObsidianInventoryItemFragment
	virtual void OnInstancedCreated(UObsidianInventoryItemInstance* Instance) const override;
	//~ End of UObsidianInventoryItemFragment
	
};
