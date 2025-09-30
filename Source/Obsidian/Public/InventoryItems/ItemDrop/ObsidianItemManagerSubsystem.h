// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project
#include "ObsidianTreasureList.h"

#include "Subsystems/WorldSubsystem.h"
#include "ObsidianItemManagerSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianItemManagerSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	void RequestDroppingItems(TArray<FObsidianDropItem>&& ItemsToDrop) const;
};
