// Copyright 2026 out of sCope - intrxx

#pragma once

#include <CoreMinimal.h>


#include "InventoryItems/Fragments/Shards/ObsidianUsableShard.h"
#include "ObsidianUsableShard_OrbOfRescription.generated.h"

/**
 * Randomly changes the Skill Implicit of an item.
 */
UCLASS(DisplayName = "Orb of Rescription")
class OBSIDIAN_API UObsidianUsableShard_OrbOfRescription : public UObsidianUsableShard
{
	GENERATED_BODY()

public:
	virtual bool OnItemUsed(AObsidianPlayerController* ItemOwner, UObsidianInventoryItemInstance* UsingInstance,
		UObsidianInventoryItemInstance* UsingOntoInstance = nullptr) override;
	virtual void OnItemUsed_UIContext(const TArray<UObsidianInventoryItemInstance*>& AllItems,
		FObsidianItemsMatchingUsableContext& OutItemsMatchingContext) override;

protected:
	bool CanUseOnItem(const UObsidianInventoryItemInstance* Instance) const;
};
