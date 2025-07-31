// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

// ~ Project


#include "Engine/DataAsset.h"
#include "ObsidianStashTabsConfig.generated.h"

class UObsidianStashTab;

USTRUCT(BlueprintType)
struct FObsidianStashTabDefinition
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UObsidianStashTab> StashTabClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(Categories = "StashTab"))
	FGameplayTag StashTag = FGameplayTag::EmptyTag;
};

/**
 * 
 */
UCLASS(BlueprintType, Const, meta = (DisplayName = "Obsidian Stash Tabs Config", ShortToolTip = "Data asset used to hold the configuration of Stash Tabs in Obsidian."))
class OBSIDIAN_API UObsidianStashTabsConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	UObsidianStashTabsConfig(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	TArray<FObsidianStashTabDefinition> GetStashTabDefinitions() const;
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	TArray<FObsidianStashTabDefinition> StashTabs;
};
