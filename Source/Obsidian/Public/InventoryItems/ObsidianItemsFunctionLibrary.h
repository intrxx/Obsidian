// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project
#include "ObsidianTypes/ItemTypes/ObsidianItemTypes.h"

#include "Kismet/BlueprintFunctionLibrary.h"
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
	static bool GetItemStats(const UObsidianInventoryItemInstance* ItemInstance, FObsidianItemStats& OutItemStats);

	/** Gets the Item Stats for provided Item Instance. Returns True if the process was successful. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Obsidian|ItemsFunctionLibrary")
	static bool GetItemStats_WithDef(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef, const int32 TemplateCurrentItemStacks, FObsidianItemStats& OutItemStats);

	/** Calculates the amount of stacks that can be added to the Item from provided Instance, takes care of calculating the limits. Will return 0 if no Item stacks can be added for some reason. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Obsidian|ItemsFunctionLibrary")
	static int32 GetAmountOfStacksAllowedToAddToItem(const AActor* Owner, const UObsidianInventoryItemInstance* AddingFromInstance, const UObsidianInventoryItemInstance* InstanceToAddTo);

	/** Calculates the amount of stacks that can be added to the Item from provided Item Definition, takes care of calculating the limits. Will return 0 if no Item stacks can be added for some reason. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Obsidian|ItemsFunctionLibrary")
	static int32 GetAmountOfStacksAllowedToAddToItem_WithDef(const AActor* Owner, const TSubclassOf<UObsidianInventoryItemDefinition>& AddingFromItemDef, const int32 AddingFromItemDefCurrentStacks, const UObsidianInventoryItemInstance* InstanceToAddTo);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Obsidian|ItemsFunctionLibrary")
	static bool IsItemUnique(const UObsidianInventoryItemInstance* ItemInstance);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Obsidian|ItemsFunctionLibrary")
	static FGameplayTag GetCategoryTagFromDraggedItem(const FDraggedItem& DraggedItem); 
};
