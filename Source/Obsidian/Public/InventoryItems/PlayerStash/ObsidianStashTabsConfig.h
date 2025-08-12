// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

// ~ Project


#include "Engine/DataAsset.h"
#include "ObsidianStashTabsConfig.generated.h"

class UObsidianStashTabWidget;
class UObsidianStashTab;

/**
 * 
 */
UENUM(BlueprintType)
enum class EObsidianStashTabType : uint8
{
	STT_None = 0 UMETA(DisplayName = "None"),
	
	STT_GridType UMETA(DisplayName = "Grid Type"),
	STT_SlotType UMETA(DisplayName = "Slot Type")
};

/**
 * 
 */
USTRUCT(BlueprintType)
struct FObsidianStashTabDefinition
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EObsidianStashTabType StashTabType = EObsidianStashTabType::STT_None;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UObsidianStashTab> StashTabClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UObsidianStashTabWidget> StashTabWidgetClass;
	
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
