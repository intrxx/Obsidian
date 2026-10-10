// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"

#include "InventoryItems/ObsidianItemContainerComponent.h"
#include "ObsidianInventoryGridItemList.h"
#include "ObsidianTypes/ItemTypes/ObsidianItemTypes.h"

#include "ObsidianInventoryComponent.generated.h"

class UObsidianPlayerStashComponent;
struct FObsidianSavedItem;

class AObsidianPlayerController;
class FGameplayDebuggerCategory_InventoryItems;
class UObInventoryItemsWidgetController;

/**
 * 
 */
USTRUCT(BlueprintType)
struct FObsidianDefaultItemTemplate
{
	GENERATED_BODY()

public:
	/** Item Definition to add. */
	UPROPERTY(EditAnywhere)
	TSubclassOf<UObsidianInventoryItemDefinition> DefaultItemDef = nullptr;

	/** Stack count of provided item to add. */
	UPROPERTY(EditAnywhere)
	int32 StackCount = 1;

	/* If left false, the item will be added to the first available slot. */
	UPROPERTY(EditAnywhere)
	bool bOverrideInventoryPosition = false;

	/** Inventory grid position at which the item will be added. */
	UPROPERTY(EditAnywhere, meta=(EditCondition="bOverrideInventoryPosition", EditConditionHides))
	FIntPoint InventoryPositionOverride = FIntPoint::NoneValue;
};

/**
 * Primary Inventory Component of Obsidian to be used by Characters.
 */
UCLASS( ClassGroup=(InventoryItems), meta=(BlueprintSpawnableComponent) )
class OBSIDIAN_API UObsidianInventoryComponent : public UObsidianItemContainerComponent
{
	GENERATED_BODY()

public:	
	UObsidianInventoryComponent(const FObjectInitializer& InObjectInitializer = FObjectInitializer::Get());

	/** Returns the Inventory Component if one exists on the specified actor, will be nullptr otherwise */
	UFUNCTION(BlueprintPure, Category = "Obsidian|Inventory")
	static UObsidianInventoryComponent* FindInventoryComponent(const AActor* InActor)
	{
		return (InActor ? InActor->FindComponentByClass<UObsidianInventoryComponent>() : nullptr);
	}

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void InitSaveData(const bool bInReceivedInitialItems);
	
	bool DidReceiveInitialInventoryItems() const;

	int32 GetInventoryGridWidth() const;
	int32 GetInventoryGridHeight() const;

	/** Finds all stacks in the inventory for given item type with item Def. */
	int32 FindAllStacksForGivenItem(const TSubclassOf<UObsidianInventoryItemDefinition>& InItemDef);
	
	/** Finds all stacks in the inventory for given item type with item Instance. */
	int32 FindAllStacksForGivenItem(const UObsidianInventoryItemInstance* InItemInstance);

	/**
	 * Gets the total amount of items added to the inventory with the same item Definition.
	 * This does not include stacks, only individual entries to the inventory.
	 */
	int32 GetTotalItemCountByDefinition(const TSubclassOf<UObsidianInventoryItemDefinition>& InItemDef) const;

	UObsidianInventoryItemInstance* GetItemInstanceAtLocation(const FIntPoint& InLocation) const;

	/** Gets all Item Instances that are added to the inventory. */
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Obsidian|Inventory")
	TArray<UObsidianInventoryItemInstance*> GetAllItems() const;

	TMap<FIntPoint, bool> GetGridStateMap() const;
	
	/** Finds first Item Instance in the inventory for provided Item Definition. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Obsidian|Inventory")
	UObsidianInventoryItemInstance* FindFirstItemInstanceForDefinition(const TSubclassOf<UObsidianInventoryItemDefinition> InItemDef) const;

	bool CanOwnerModifyInventoryState();
	
	/** Checks if the item fits in the provided spot. */
	bool CheckSpecifiedPosition(const FIntPoint& InItemGridSpan, const FIntPoint& InSpecifiedPosition);
	
     /**
	 * Will try to add provided amount of stacks of provided Item to any of the same Item present in the Inventory.
	 * Returns Array of Instances that stacks were added to.
	 *
	 *	@param AddingFromItemDef		The Item Definition that the function will try to add from.
	 *	@param StacksToAdd				The Current Stacks of the provided Item Definition.
	 *  @param OutAddedToInstances		Array of Inventory Item Instances that stacks were added to.
	 *
	 *  @return The struct that contains various useful information about the result of the adding process.
	 */
	FObsidianAddingStacksResult TryAddingStacksToExistingItems(const TSubclassOf<UObsidianInventoryItemDefinition>& InAddingFromItemDef,
		const int32 InStacksToAdd, TArray<UObsidianInventoryItemInstance*>& OutAddedToInstances);
	
	/** Checks if the provided Item Definition can replace item at provided slot. */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|Inventory")
	bool CanReplaceItemAtSpecificSlotWithDef(const FIntPoint& InClickedInstancePosition,
		const FIntPoint& InClickedGridPosition, const TSubclassOf<UObsidianInventoryItemDefinition> InItemDef,
		const int32 InStackCount = 1);

	/** Checks if the provided Item Definition fits anywhere in the inventory. */
	bool CanFitItemDefinition(const TSubclassOf<UObsidianInventoryItemDefinition>& InItemDef);
	
	/**
	 * Tries to add Item Definition to the inventory, if the item is stackable will first try to add all the stacks
	 * to the same item types if they exist in inventory.
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|Inventory")
	FObsidianItemOperationResult AddItemDefinition(const TSubclassOf<UObsidianInventoryItemDefinition> InItemDef,
		const FObsidianItemGeneratedData& InItemGeneratedData);

	/** Tries to add provided Item Definition to provided Slot. */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|Inventory")
	FObsidianItemOperationResult AddItemDefinitionToSpecifiedSlot(const TSubclassOf<UObsidianInventoryItemDefinition> InItemDef,
		const FIntPoint& InToGridSlot, const FObsidianItemGeneratedData& InItemGeneratedData, const int32 InStackToAddOverride = -1);

	/**
	 *	Will try to add stacks from provided Item Definition at provided Position.
	 *
	 *	@param AddingFromItemDef				The Item Definition that the function will try to add from.
	 *	@param AddingFromItemDefCurrentStacks	The Current Stacks of the provided Item Definition.
	 *  @param AtPosition						The pressed slot at which the function will search for item to add to.			
	 *  @param StackToAddOverride				Optional override for the amount of stacks the function will try to add to the item. Will do nothing if unused.
	 *
	 *  @return The struct that contains various useful information about the result of the adding process.
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|Inventory")
	FObsidianAddingStacksResult TryAddingStacksToSpecificSlotWithItemDef(
		const TSubclassOf<UObsidianInventoryItemDefinition>& InAddingFromItemDef, const int32 InAddingFromItemDefCurrentStacks,
		const FIntPoint& InAtPosition, const int32 InStackToAddOverride = -1);
	
	/** Checks if the provided Item Instance can replace item at provided slot. */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|Inventory")
	bool CanReplaceItemAtSpecificSlotWithInstance(const FIntPoint& InClickedInstancePosition,
		const FIntPoint& InClickedGridPosition, UObsidianInventoryItemInstance* InReplacingInstance);

	/** Checks if the provided Item Instance fits anywhere in the inventory. */
	bool CanFitItemInstance(const UObsidianInventoryItemInstance* InInstance);
	
	/**
	 * Tries to add Item Instance to the inventory, if the item is stackable will first try to add all the stacks
	 * to the same item types if they exist in inventory.
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|Inventory")
	FObsidianItemOperationResult AddItemInstance(UObsidianInventoryItemInstance* InInstanceToAdd);

	/** Tries to add provided Item Instance to provided Slot. */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|Inventory")
	FObsidianItemOperationResult AddItemInstanceToSpecificSlot(UObsidianInventoryItemInstance* InInstanceToAdd,
		const FIntPoint& InToGridSlot, const int32 InStackToAddOverride = -1);
	
	/**
	 *	Will try to add stacks from provided Item Instance at provided Position. 
	 *
	 *	@param AddingFromInstance		The Item Instance that the function will try to add from.
	 *  @param AtGridSlot				The pressed slot at which the function will search for item to add to.
	 *  @param StackToAddOverride		Optional override for the amount of stacks the function will try to add to the item. Will do nothing if unused.
	 *
	 *  @return The struct that contains various useful information about the result of the adding process.
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|Inventory")
	FObsidianAddingStacksResult TryAddingStacksToSpecificSlotWithInstance(UObsidianInventoryItemInstance* InAddingFromInstance,
		const FIntPoint& InAtGridSlot, const int32 InStackToAddOverride = -1);

	/**
	 *	Provides a copied Item with the amount of stacks to take. Shouldn't ever be called to take out full item stacks or 0 stacks.
	 *
	 *	@param TakingFromInstance	Item Instance that the function will take from, essentially duplicating it.
	 *	@param StacksToTake			Stacks to take from provided Item Instance, it is also clamped between 1 and ItemCurrentStacks - 1.
	 *
	 *	@return New, duplicated item instance with StacksToTake number of stacks.
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|Inventory")
	FObsidianItemOperationResult TakeOutFromItemInstance(UObsidianInventoryItemInstance* InTakingFromInstance,
		const int32 InStacksToTake);

	/** Removes Item Instance from inventory. */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|Inventory")
	FObsidianItemOperationResult RemoveItemInstance(UObsidianInventoryItemInstance* InInstanceToRemove);


	
	void LoadInventorizedItem(const FObsidianSavedItem& InInventorizedSavedItem);
	
	//~ Start of UObsidianItemContainerComponent interface
	virtual TArray<UObsidianInventoryItemInstance*> GetContainedItems() const override;
	//~ End of UObsidianItemContainerComponent interface

protected:
	virtual void BeginPlay() override;
	
	//~ Start of UObsidianItemContainerComponent interface
	virtual FGameplayTag GetBlockActionsTag() const override;
	virtual void AddItemInstanceToList(UObsidianInventoryItemInstance* InInstance, const FObsidianItemPosition& InToPosition) override;
	virtual UObsidianInventoryItemInstance* AddItemDefinitionToList(const TSubclassOf<UObsidianInventoryItemDefinition>& InItemDef,
		const FObsidianItemGeneratedData& InItemGeneratedData, const int32 InStackCount, const FObsidianItemPosition& InToPosition) override;
	virtual void RemoveItemInstanceFromList(UObsidianInventoryItemInstance* InInstance) override;
	virtual void HandleItemStacksChanged(UObsidianInventoryItemInstance* InInstance, const int32 InOldStackCount) override;
	virtual void HandleItemChanged(UObsidianInventoryItemInstance* InInstance) override;
	//~ End of UObsidianItemContainerComponent interface

protected:
	UPROPERTY(EditAnywhere, Category = "Obsidian|Default")
	TArray<FObsidianDefaultItemTemplate> DefaultInventoryItems;

private:
	/** Initializes Inventory State. */
	void InitInventoryState();

	/** Adds default items specified in DefaultInventoryItems. */
	void AddDefaultItems();
	
	/** Checks if the provided Item Definition fits anywhere in the inventory. Provides Available Position. */
	bool CanFitItemDefinition(FIntPoint& OutAvailablePositions, const TSubclassOf<UObsidianInventoryItemDefinition>& InItemDef);

	/** Checks if the provided Item Definition fits in the inventory at provided slot. */
	bool CanFitItemDefinitionToSpecifiedSlot(const FIntPoint& InSpecifiedSlot,
		const TSubclassOf<UObsidianInventoryItemDefinition>& InItemDef);

	bool CheckReplacementPossible(const FIntPoint& InItemToReplaceOriginPosition, const FIntPoint& InAtGridSlot,
		const FIntPoint& InGridSpanAtPosition, const FIntPoint& InReplacingGridSpan) const;
	
	/** Checks the limit of the item, returns the number of stacks available to add to the inventory with provided ItemDef. */
	int32 GetNumberOfStacksAvailableToAddToInventory(const TSubclassOf<UObsidianInventoryItemDefinition>& InItemDef,
		const int32 InCurrentStacks);

	/** Checks the limit of the item, returns the number of stacks available to add to the inventory with provided instance. */
	int32 GetNumberOfStacksAvailableToAddToInventory(const UObsidianInventoryItemInstance* InItemInstance);

	/** Gets the item location from internal grid map. */
	FIntPoint GetItemLocationFromGrid(UObsidianInventoryItemInstance* InItemInstance) const;
	
	/** Checks if the item fits in the inventory, outputs the first available position.  */
	bool CheckAvailablePosition(FIntPoint& OutAvailablePosition, const FIntPoint& InItemGridSpan);
	
	/** Internal usage only, this returns the internal Location To Instance Map. */
	TMap<FIntPoint, UObsidianInventoryItemInstance*> Internal_GetLocationToInstanceMap();

private:
	friend UObInventoryItemsWidgetController;

#if WITH_GAMEPLAY_DEBUGGER
	friend FGameplayDebuggerCategory_InventoryItems;
#endif
	
	/**
	 * Actual array of items which is FFastArraySerializer.
	 * It also contains Map which maps Grid Vector2D location to actual Item Instance in the inventory.
	 */
	UPROPERTY(Replicated)
	FObsidianInventoryGridItemList InventoryGrid;
	
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|InventorySetup")
	int32 InventoryGridWidth = 12;
	
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|InventorySetup")
	int32 InventoryGridHeight = 5;

	/** Grid size of the inventory, calculated (InventoryGridWidth * InventoryGridHeight). */
	int32 InventoryGridSize = 0;



	UPROPERTY(Replicated)
	bool bReceivedInitialInventoryItems = false;
	FDelegateHandle AddDefaultItemsDelegateHandle;
};
