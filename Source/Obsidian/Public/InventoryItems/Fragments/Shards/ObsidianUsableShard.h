// Copyright 2026 out of sCope team - intrxx

#pragma once

#include <CoreMinimal.h>

#include "ObsidianTypes/ItemTypes/ObsidianItemTypes.h"

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

public:
	void AddMatchingItem(const UObsidianInventoryItemInstance* InstanceToAdd);
	bool HasAnyMatchingItems() const;
	
public:
	UPROPERTY()
	TArray<FObsidianItemPosition> InventoryItemsMatchingContext;

	UPROPERTY()
	TArray<FObsidianItemPosition> EquipmentItemsMatchingContext;
	
	TMultiMap<FGameplayTag, FObsidianItemPosition> StashItemsMatchingContext;
};

/**
 * Represent some OnItemUse logic for an Item.
 */
UCLASS(DefaultToInstanced, EditInlineNew, Abstract)
class OBSIDIAN_API UObsidianUsableShard : public UObject
{
	GENERATED_BODY()
	
public:
	virtual bool OnItemUsed(AObsidianPlayerController* ItemOwner, UObsidianInventoryItemInstance* UsingInstance,
		UObsidianInventoryItemInstance* UsingOntoInstance = nullptr);
	virtual void OnItemUsed_UIContext(const TArray<UObsidianInventoryItemInstance*>& AllItems,
		FObsidianItemsMatchingUsableContext& OutItemsMatchingContext);
};
