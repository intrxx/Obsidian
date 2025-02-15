// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "InventoryItems/ObsidianInventoryItemFragment.h"
#include "OInventoryItemFragment_Stacks.generated.h"

/**
 * 
 */
UCLASS(DisplayName="Stacks")
class OBSIDIAN_API UOInventoryItemFragment_Stacks : public UObsidianInventoryItemFragment
{
	GENERATED_BODY()

public:
	//~ Start of UObsidianInventoryItemFragment
	virtual void OnInstancedCreated(UObsidianInventoryItemInstance* Instance) const override;
	//~ End of UObsidianInventoryItemFragment
	
	/** Getter for item stack number, if the Tag for Stack Count does not exist on the item, it will return the unified project default. */
	int32 GetItemStackNumberByTag(const FGameplayTag Tag) const;
	
	bool IsStackable() const
	{
		return bStackable;
	}

protected:
	/** 
	 * Whether, or not this item is stackable, needs to be set if the item wishes to display stacks and be added to other items.
	 * This might not be set if the item has some inventory limit set but is not stackable, e.g. non-stackable quest items limited to 3.
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Stacks")
	bool bStackable = true;
	
	UPROPERTY(EditDefaultsOnly, Category = "Stacks")
	TMap<FGameplayTag, int32> InventoryItemStackNumbers;
};
