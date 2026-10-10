// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "ObsidianTypes/ItemTypes/ObsidianItemTypes.h"

#include "ObsidianItemsFunctionLibrary.generated.h"

class AObsidianPlayerController;
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
	static const UObsidianInventoryItemFragment* FindItemDefinitionFragment(const TSubclassOf<UObsidianInventoryItemDefinition> InItemDef,
		const TSubclassOf<UObsidianInventoryItemFragment> InFragmentClass);

	/** Will compare item's definitions, will return true if items are of the same class. */
	UFUNCTION(BlueprintCallable, Category = "Obsidian|ItemsFunctionLibrary")
	static bool IsTheSameItem(const UObsidianInventoryItemInstance* InInstanceA, const UObsidianInventoryItemInstance* InInstanceB);

	/** Will compare item's definitions, will return true if items are of the same class. */
	UFUNCTION(BlueprintCallable, Category = "Obsidian|ItemsFunctionLibrary")
	static bool IsTheSameItem_WithDef(const UObsidianInventoryItemInstance* InInstance, const TSubclassOf<UObsidianInventoryItemDefinition>& InItemDef);
	
	/** Formats Item Affixes into Affix Row for Item Description. */
	static TArray<FObsidianAffixDescriptionRow> FormatItemAffixes(const TArray<FObsidianActiveItemAffix>& InItemAffixes);
	static TArray<FObsidianAffixDescriptionRow> FormatUnidentifiedItemAffixes(const TArray<FObsidianActiveItemAffix>& InItemAffixes);
	
	static bool HasEquippingRequirements(const FObsidianItemRequirements& InRequirements);

	static FObsidianDynamicItemAffix GetRandomSkillImplicitForItem(const UObsidianInventoryItemInstance* InForItem);
	static FObsidianDynamicItemAffix GetRandomImplicitForItem(const UObsidianInventoryItemInstance* InForItem);
	static FObsidianDynamicItemAffix GetRandomPrefixForItem(const UObsidianInventoryItemInstance* InForItem);
	static FObsidianDynamicItemAffix GetRandomSuffixForItem(const UObsidianInventoryItemInstance* InForItem);
	static FObsidianDynamicItemAffix GetRandomDynamicAffix(const TArray<FObsidianDynamicItemAffix>& InDynamicAffixes);
	
	static bool FillItemGeneratedData(FObsidianItemGeneratedData& OutGeneratedData, const UObsidianInventoryItemInstance* InFromInstance);
	static void InitializeItemInstanceWithGeneratedData(UObsidianInventoryItemInstance* InInstance, const FObsidianItemGeneratedData& InGeneratedData);

	/**
	 * Creates new Item Instance from Item Definition, initialized by its Fragments and provided Generated Data.
	 * This does not set the stack count or the identification, as it is up to the caller how many stacks the new item holds.
	 */
	static UObsidianInventoryItemInstance* CreateItemInstanceFromDefinition(UObject* InOuter, const TSubclassOf<UObsidianInventoryItemDefinition>& InItemDefClass,
		const FObsidianItemGeneratedData& InItemGeneratedData, const FObsidianItemPosition& InAtPosition);

	/** Gets the Item Stats for provided Item Instance. Returns True if the process was successful. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Obsidian|ItemsFunctionLibrary")
	static bool GetItemStats(const AObsidianPlayerController* InOwnerPC, const UObsidianInventoryItemInstance* InItemInstance, FObsidianItemStats& OutItemStats);
	
	/** Gets the Item Stats for provided Item Instance. Returns True if the process was successful. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Obsidian|ItemsFunctionLibrary")
	static bool GetItemStats_WithDef(const AObsidianPlayerController* InOwnerPC, const TSubclassOf<UObsidianInventoryItemDefinition>& InItemDef,
		const FObsidianItemGeneratedData& InItemGeneratedData, FObsidianItemStats& OutItemStats);

	static bool GenerateItemEquippingRequirementsAsUIDesc(const AObsidianPlayerController* InOwnerPC, const FObsidianItemRequirements& InRequirements,
		FObsidianItemRequirementsUIDescription& OutRequirementsUIDescription);
	
	/** Calculates the amount of stacks that can be added to the Item from provided Instance, takes care of calculating the limits. Will return 0 if no Item stacks can be added for some reason. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Obsidian|ItemsFunctionLibrary")
	static int32 GetAmountOfStacksAllowedToAddToItem(const AActor* InOwner, const UObsidianInventoryItemInstance* InAddingFromInstance,
		const UObsidianInventoryItemInstance* InInstanceToAddTo);

	/**
	 * Calculates the amount of stacks that can be added to the Item from provided Item Definition, takes care of calculating the limits.
	 * Will return 0 if no Item stacks can be added for some reason.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Obsidian|ItemsFunctionLibrary")
	static int32 GetAmountOfStacksAllowedToAddToItem_WithDef(const AActor* InOwner, const TSubclassOf<UObsidianInventoryItemDefinition>& InAddingFromItemDef,
		const int32 InAddingFromItemDefCurrentStacks, const UObsidianInventoryItemInstance* InInstanceToAddTo);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Obsidian|ItemsFunctionLibrary")
	static bool IsItemUnique(const UObsidianInventoryItemInstance* InItemInstance);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Obsidian|ItemsFunctionLibrary")
	static bool IsTwoHanded(const UObsidianInventoryItemInstance* InItemInstance);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Obsidian|ItemsFunctionLibrary")
	static bool IsTwoHanded_WithCategory(const FGameplayTag& InCategoryTag);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Obsidian|ItemsFunctionLibrary")
	static FGameplayTag GetCategoryTagFromDraggedItem(const FDraggedItem& InDraggedItem);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Obsidian|ItemsFunctionLibrary")
	static FGameplayTag GetBaseTypeTagFromDraggedItem(const FDraggedItem& InDraggedItem);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Obsidian|ItemsFunctionLibrary")
	static bool GetItemCategoryAndBaseItemTypeTagsFromDraggedItem(const FDraggedItem& InDraggedItem, FGameplayTag& OutCategoryTag,
		FGameplayTag& OutItemBaseTypeTag);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Obsidian|ItemsFunctionLibrary")
	static FIntPoint GetGridSpanFromDraggedItem(const FDraggedItem& InDraggedItem);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Obsidian|ItemsFunctionLibrary")
	static bool IsDefinitionIdentified(const UObsidianInventoryItemDefinition* InItemDefault, const FObsidianItemGeneratedData& InItemGeneratedData);
};
