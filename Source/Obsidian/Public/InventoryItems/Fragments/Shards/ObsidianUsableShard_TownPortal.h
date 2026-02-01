// Copyright 2026 out of sCope team - intrxx

#pragma once

#include <CoreMinimal.h>


#include "InventoryItems/Fragments/Shards/ObsidianUsableShard.h"
#include "ObsidianUsableShard_TownPortal.generated.h"

class AObsidianTownPortal;

/**
 * Creates a Portal that leads to current area HUB (Town).
 */
UCLASS(DisplayName="Town Portal")
class OBSIDIAN_API UObsidianUsableShard_TownPortal : public UObsidianUsableShard
{
	GENERATED_BODY()

public:
	virtual bool OnItemUsed(AObsidianPlayerController* ItemOwner, UObsidianInventoryItemInstance* UsingInstance,
		UObsidianInventoryItemInstance* UsingOntoInstance = nullptr) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Obsidian")
	FTransform SpawnTransform;

	//TODO(intrxx) Replace with TSoftClassPtr<>
	UPROPERTY(EditAnywhere, Category = "Obsidian")
	TSubclassOf<AObsidianTownPortal> TownPortalActorClass;
};
