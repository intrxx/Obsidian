// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ObsidianTypes/ObsidianItemTypes.h"
#include "ObsidianItemsFunctionLibrary.generated.h"

class UObsidianInventoryItemFragment;
class UObsidianInventoryItemDefinition;
class UObsidianInventoryItemInstance;

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianItemsFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** Will try to Find Item Fragment on provided Item Definition. */
	UFUNCTION(BlueprintCallable, meta=(DeterminesOutputType = FragmentClass), Category = "Obsidian|ItemsFunctionLibrary")
	static const UObsidianInventoryItemFragment* FindItemDefinitionFragment(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef, const TSubclassOf<UObsidianInventoryItemFragment> FragmentClass);

	/** Will compare item's definitions, will return true if items are of the same class. */
	UFUNCTION(BlueprintCallable, Category = "Obsidian|ItemsFunctionLibrary")
	static bool IsTheSameItem(const UObsidianInventoryItemInstance* InstanceA, const UObsidianInventoryItemInstance* InstanceB);

	/** Will compare item's definitions, will return true if items are of the same class. */
	UFUNCTION(BlueprintCallable, Category = "Obsidian|ItemsFunctionLibrary")
	static bool IsTheSameItem_WithDef(const UObsidianInventoryItemInstance* Instance, const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef);

	/** Gets the Item Stats for provided Item Instance. Returns True if the process was successful. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Obsidian|ItemsFunctionLibrary")
	static bool GetItemStatsForItemInstance(const UObsidianInventoryItemInstance* ItemInstance, FObsidianItemStats& OutItemStats);

	/** Gets the Item Stats for provided Item Instance. Returns True if the process was successful. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Obsidian|ItemsFunctionLibrary")
	static bool GetItemStatsForItemDefinition(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef, const int32 TemplateCurrentItemStacks, FObsidianItemStats& OutItemStats);
};
