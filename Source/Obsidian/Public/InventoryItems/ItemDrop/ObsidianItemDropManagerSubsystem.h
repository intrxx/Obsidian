// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"

#include "ObsidianTreasureList.h"

#include "ObsidianItemDropManagerSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianItemDropManagerSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	void RequestDroppingItems(TArray<FObsidianItemToDrop>&& InItemsToDrop) const;
};
