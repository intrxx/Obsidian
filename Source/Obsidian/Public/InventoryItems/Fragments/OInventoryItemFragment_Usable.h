// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project


#include "InventoryItems/ObsidianInventoryItemFragment.h"
#include "OInventoryItemFragment_Usable.generated.h"

class UObsidianUsableShard;

/**
 * Fragment that allows the item to have Usable functionality.
 */
UCLASS(DisplayName="Usable")
class OBSIDIAN_API UOInventoryItemFragment_Usable : public UObsidianInventoryItemFragment
{
	GENERATED_BODY()
	
public:
	//~ Start of UObsidianInventoryItemFragment
	virtual void OnInstancedCreated(UObsidianInventoryItemInstance* Instance) const override;
	//~ End of UObsidianInventoryItemFragment

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Usable")
	TObjectPtr<UObsidianUsableShard> UsableShard = nullptr;
};
