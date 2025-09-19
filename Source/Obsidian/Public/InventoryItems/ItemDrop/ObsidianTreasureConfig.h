// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project

#include "Engine/DataAsset.h"
#include "ObsidianTreasureConfig.generated.h"

class UObsidianTreasureList;

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianTreasureConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	TArray<TObjectPtr<UObsidianTreasureList>> CommonTreasureLists;
};
