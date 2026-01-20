// Copyright 2026 out of sCope team - intrxx

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project


#include "InventoryItems/Fragments/Shards/ObsidianUsableShard.h"
#include "ObsidianUsableShard_TownPortal.generated.h"

class AObsidianTownPortal;

/**
 * 
 */
UCLASS(DisplayName="Town Portal")
class OBSIDIAN_API UObsidianUsableShard_TownPortal : public UObsidianUsableShard
{
	GENERATED_BODY()

public:
	virtual bool OnItemUsed(AObsidianPlayerController* ItemOwner, UObsidianInventoryItemInstance* UsingInstance, UObsidianInventoryItemInstance* UsingOntoInstance = nullptr) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Obsidian")
	FTransform SpawnTransform;
	
	UPROPERTY(EditAnywhere, Category = "Obsidian")
	TSubclassOf<AObsidianTownPortal> TownPortalActorClass;
};
