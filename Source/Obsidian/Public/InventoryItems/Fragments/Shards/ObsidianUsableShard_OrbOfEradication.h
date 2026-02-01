// Copyright 2026 out of sCope - intrxx

#pragma once

#include <CoreMinimal.h>


#include "InventoryItems/Fragments/Shards/ObsidianUsableShard.h"
#include "ObsidianUsableShard_OrbOfEradication.generated.h"

/**
 * Removes all prefixes and suffixes from an item, switching its rarity to normal.
 */
UCLASS(DisplayName = "Orb of Eradication")
class OBSIDIAN_API UObsidianUsableShard_OrbOfEradication : public UObsidianUsableShard
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
