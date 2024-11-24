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

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Stacks")
	TMap<FGameplayTag, int32> InventoryItemStackNumbers;
};
