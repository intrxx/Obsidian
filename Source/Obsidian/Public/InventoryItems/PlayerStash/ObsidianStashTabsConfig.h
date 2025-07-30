// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project


#include "Engine/DataAsset.h"
#include "ObsidianStashTabsConfig.generated.h"

class UObsidianStashTab;

USTRUCT(BlueprintType)
struct FObsidianStashTabDefinition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced)
	TObjectPtr<UObsidianStashTab> StashTab;
};

/**
 * 
 */
UCLASS(BlueprintType, Const, meta = (DisplayName = "Obsidian Stash Tabs Config", ShortToolTip = "Data asset used to hold the configuration of Stash Tabs in Obsidian."))
class OBSIDIAN_API UObsidianStashTabsConfig : public UDataAsset
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	TArray<FObsidianStashTabDefinition> StashTabs;
};
