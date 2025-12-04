// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include <CoreMinimal.h>

#include "ObsidianTreasureList.h"

#include <Subsystems/WorldSubsystem.h>
#include "ObsidianItemDropManagerSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianItemDropManagerSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	void RequestDroppingItems(TArray<FObsidianItemToDrop>&& ItemsToDrop) const;
};
