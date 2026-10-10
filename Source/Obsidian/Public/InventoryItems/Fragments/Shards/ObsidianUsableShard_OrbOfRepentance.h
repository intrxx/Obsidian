// Copyright 2026 out of sCope - intrxx

#pragma once

#include "CoreMinimal.h"

#include "InventoryItems/Fragments/Shards/ObsidianUsableShard.h"

#include "ObsidianUsableShard_OrbOfRepentance.generated.h"

/**
 * Removes random prefix or suffix from an item.
 */
UCLASS(DisplayName = "Orb of Repentance")
class OBSIDIAN_API UObsidianUsableShard_OrbOfRepentance : public UObsidianUsableShard
{
	GENERATED_BODY()

public:
	virtual bool OnItemUsed(AObsidianPlayerController* InItemOwner, UObsidianInventoryItemInstance* InUsingInstance,
		UObsidianInventoryItemInstance* InUsingOntoInstance = nullptr) override;
	virtual void OnItemUsed_UIContext(const TArray<UObsidianInventoryItemInstance*>& InAllItems,
		FObsidianItemsMatchingUsableContext& OutItemsMatchingContext) override;

protected:
	bool CanUseOnItem(const UObsidianInventoryItemInstance* InInstance) const;
};
