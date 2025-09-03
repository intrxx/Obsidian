// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

// ~ Project
#include "ObsidianTypes/ObsidianItemTypes.h"

#include "ObsidianUsableShard.generated.h"

class AObsidianPlayerController;
class UObsidianInventoryComponent;
class UObsidianInventoryItemInstance;

/**
 * 
 */
USTRUCT(BlueprintType)
struct FObsidianItemsMatchingUsableContext
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FIntPoint> InventoryItemsMatchingContext;

	UPROPERTY()
	TArray<FGameplayTag> EquipmentItemsMatchingContext;

	UPROPERTY()
	TArray<FObsidianItemPosition> StashItemsMatchingContext;
};

/**
 * Represent some OnItemUse logic for an Item.
 */
UCLASS(DefaultToInstanced, EditInlineNew, Abstract)
class OBSIDIAN_API UObsidianUsableShard : public UObject
{
	GENERATED_BODY()
	
public:
	virtual bool OnItemUsed(AObsidianPlayerController* ItemOwner, UObsidianInventoryItemInstance* UsingInstance, UObsidianInventoryItemInstance* UsingOntoInstance = nullptr);
	virtual FObsidianItemsMatchingUsableContext OnItemUsed_UIContext(const TArray<UObsidianInventoryItemInstance*>& AllItems);
};
