// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "ObsidianInventoryGrid.h"
#include "Components/ActorComponent.h"
#include "ObsidianTypes/ObsidianItemTypes.h"
#include "ObsidianInventoryComponent.generated.h"

class FGameplayDebuggerCategory_InventoryItems;
class UObsidianInventoryWidgetController;

DECLARE_LOG_CATEGORY_EXTERN(LogInventory, Log, All);

USTRUCT(BlueprintType)
struct FObsidianDefaultItemTemplate
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UObsidianInventoryItemDefinition> DefaultItemDef = nullptr;

	UPROPERTY(EditAnywhere)
	int32 StackCount = 1;

	UPROPERTY(EditAnywhere)
	FVector2D InventoryPositionOverride = FVector2D(-1.0f, -1.0f);
};

/**
 * Primary Inventory Component of Obsidian to be used by Characters.
 */
UCLASS( ClassGroup=(InventoryItems), meta=(BlueprintSpawnableComponent) )
class OBSIDIAN_API UObsidianInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UObsidianInventoryComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** Returns the Inventory Component if one exists on the specified actor, will be nullptr otherwise */
	UFUNCTION(BlueprintPure, Category = "Obsidian|Inventory")
	static UObsidianInventoryComponent* FindInventoryComponent(const AActor* Actor)
	{
		return (Actor ? Actor->FindComponentByClass<UObsidianInventoryComponent>() : nullptr);
	}

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** Gets the total amount of items added to the inventory with the same item Definition. This does not include stacks, only individual entries to the inventory. */
	int32 GetTotalItemCountByDefinition(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef) const;

	UObsidianInventoryItemInstance* GetItemInstanceAtLocation(const FVector2D& Location) const;

	/** Gets all Item Instances that are added to the inventory. */
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Obsidian|Inventory")
	TArray<UObsidianInventoryItemInstance*> GetAllItems() const;

	/** Gets the Item Stats based on provided Item position on the Inventory Grid. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Obsidian|Inventory")
	FObsidianItemStats GetItemStatsByInventoryPosition(const FVector2D& InPosition) const;

	/** Gets the Item Stats from provided Item Instance. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Obsidian|Inventory")
	FObsidianItemStats GetItemStatForInstance(const UObsidianInventoryItemInstance* ItemInstance) const;

	/** Gets the Item Stats from provided Item Definition. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Obsidian|Inventory")
	FObsidianItemStats GetItemStatsForItemDefinition(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef, const int32 CurrentItemStacks) const;
	 
	/** Finds first Item Instance in the inventory for provided Item Definition. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Obsidian|Inventory")
	UObsidianInventoryItemInstance* FindFirstItemInstanceForDefinition(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef) const;

	/** Checks if the item fits in the provided spot. */
	bool CheckSpecifiedPosition(const TArray<FVector2D>& ItemGridSize, const FVector2D& SpecifiedPosition);
	
     /**
	 * Will try to add provided amount of stacks of provided Item to any of the same Item present in the Inventory. Returns Array of Instances that stacks were added to.
	 *
	 *	@param AddingFromItemDef		The Item Definition that the function will try to add from.
	 *	@param StacksToAdd				The Current Stacks of the provided Item Definition.
	 *  @param OutAddingStacksResult	The struct that contains various useful information about the result of the adding process.
	 *
	 *  @return Array of Inventory Item Instances that stacks were added to.
	 */
	TArray<UObsidianInventoryItemInstance*> TryAddingStacksToExistingItems(const TSubclassOf<UObsidianInventoryItemDefinition>& AddingFromItemDef, const int32 StacksToAdd, FObsidianAddingStacksResult& OutAddingStacksResult);
	
	/** Checks if the provided Item Definition can replace item at provided slot. */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|Inventory")
	bool CanReplaceItemAtSpecificSlotWithDef(const FVector2D& Slot, const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef, const int32 StackCount = 1);

	/** Tries to add Item Definition to the inventory, if the item is stackable will first try to add all the stacks to the same item types if they exist in inventory. */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|Inventory")
	UObsidianInventoryItemInstance* AddItemDefinition(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef, int32& OutStacksLeft, const int32 StackCount = 1);

	/** Tries to add provided Item Definition to provided Slot. */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|Inventory")
	UObsidianInventoryItemInstance* AddItemDefinitionToSpecifiedSlot(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef, const FVector2D& ToSlot, int32& StacksLeft, const int32 StackCount = 1, const int32 StackToAddOverride = -1);

	/**
	 *	Will try to add stacks from provided Item Definition at provided Position.
	 *
	 *	@param AddingFromItemDef				The Item Definition that the function will try to add from.
	 *	@param AddingFromItemDefCurrentStacks	The Current Stacks of the provided Item Definition.
	 *  @param AtPosition						The pressed slot at which the function will search for item to add to.
	 *  @param OutAddingStacksResult			The struct that contains various useful information about the result of the adding process.
	 *  @param StackToAddOverride				Optional override for the amount of stacks the function will try to add to the item. Will do nothing if unused.
	 *
	 *  @return True if at least one stack was added successfully.
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|Inventory")
	bool TryAddingStacksToSpecificSlotWithItemDef(const TSubclassOf<UObsidianInventoryItemDefinition>& AddingFromItemDef, const int32 AddingFromItemDefCurrentStacks, const FVector2D& AtPosition, FObsidianAddingStacksResult& OutAddingStacksResult, const int32 StackToAddOverride = -1);
	
	/** Checks if the provided Item Instance can replace item at provided slot. */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|Inventory")
	bool CanReplaceItemAtSpecificSlotWithInstance(const FVector2D& Slot, UObsidianInventoryItemInstance* ReplacingInstance);

	/** Tries to add Item Instance to the inventory, if the item is stackable will first try to add all the stacks to the same item types if they exist in inventory. */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|Inventory")
	void AddItemInstance(UObsidianInventoryItemInstance* InstanceToAdd, int32& OutStacksLeft);

	/** Tries to add provided Item Instance to provided Slot. */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|Inventory")
	bool AddItemInstanceToSpecificSlot(UObsidianInventoryItemInstance* InstanceToAdd, const FVector2D& ToSlot, const int32 StackToAddOverride = -1);
	
	/**
	 *	Will try to add stacks from provided Item Instance at provided Position. 
	 *
	 *	@param AddingFromInstance		The Item Instance that the function will try to add from.
	 *  @param AtPosition				The pressed slot at which the function will search for item to add to.
	 *  @param OutAddingStacksResult	The struct that contains various useful information about the result of the adding process.
	 *  @param StackToAddOverride		Optional override for the amount of stacks the function will try to add to the item. Will do nothing if unused.
	 *
	 *  @return True if at least one stack was added successfully.
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|Inventory")
	bool TryAddingStacksToSpecificSlotWithInstance(UObsidianInventoryItemInstance* AddingFromInstance, const FVector2D& AtPosition, FObsidianAddingStacksResult& OutAddingStacksResult, const int32 StackToAddOverride = -1);

	/**
	 *	Provides a copied Item with the amount of stacks to take. Shouldn't ever be called to take out full item stacks or 0 stacks.
	 *
	 *	@param TakingFromInstance	Item Instance that the function will take from, essentially duplicating it.
	 *	@param StacksToTake			Stacks to take from provided Item Instance, it is also clamped between 1 and ItemCurrentStacks - 1.
	 *
	 *	@return New, duplicated item instance with StacksToTake number of stacks.
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|Inventory")
	UObsidianInventoryItemInstance* TakeOutFromItemInstance(UObsidianInventoryItemInstance* TakingFromInstance, const int32 StacksToTake);

	/** Removes Item Instance from inventory. */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|Inventory")
	void RemoveItemInstance(UObsidianInventoryItemInstance* InstanceToRemove);

	/** Consumes every item in the inventory that matches provided Item Definition. */
	bool ConsumeItemsByDefinition(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef, const int32 NumberOfItemsToConsume);
	
	//~ Start of UObject interface
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	virtual void ReadyForReplication() override;
	//~ End of UObject interface

protected:
	virtual void BeginPlay() override;
	
	bool IsLocallyControlled();

protected:
	UPROPERTY(EditAnywhere, Category = "Obsidian|Default")
	TArray<FObsidianDefaultItemTemplate> DefaultInventoryItems;

private:
	/** Initializes Inventory State. */
	void InitInventoryState();

	/** Checks if the provided Item Definition fits anywhere in the inventory. Provides Available Position. */
	bool CanFitItemDefinition(FVector2D& OutAvailablePosition, const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef);

	/** Checks if the provided Item Definition fits in the inventory at provided slot. */
	bool CanFitItemDefinitionToSpecifiedSlot(const FVector2D& SpecifiedSlot, const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef);

	/** Checks if the provided Item Instance fits anywhere in the inventory. Provides Available Position. */
	bool CanFitItemInstance(FVector2D& OutAvailablePosition, UObsidianInventoryItemInstance* Instance);
	
	/** Checks if the provided Item Instance fits in the inventory at provided slot. */
	bool CanFitItemInstanceToSpecificSlot(const FVector2D& SpecifiedSlot, UObsidianInventoryItemInstance* Instance);
	
	/** Finds all stacks in the inventory for given item type with item Def. */
	int32 FindAllStacksForGivenItem(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef);
	
	/** Finds all stacks in the inventory for given item type with item Instance. */
	int32 FindAllStacksForGivenItem(const UObsidianInventoryItemInstance* ItemInstance);

	/** Calculates the amount of stacks that can be added to the Item from provided Instance, takes care of calculating the limits. Will return 0 if no Item stacks can be added for some reason. */
	int32 GetAmountOfStacksAvailableToAddToItem(const UObsidianInventoryItemInstance* AddingFromInstance, const UObsidianInventoryItemInstance* InstanceToAddTo);

	/** Calculates the amount of stacks that can be added to the Item from provided Item Definition, takes care of calculating the limits. Will return 0 if no Item stacks can be added for some reason. */
	int32 GetAmountOfStacksAvailableToAddToItem(const TSubclassOf<UObsidianInventoryItemDefinition>& AddingFromItemDef, const int32 AddingFromItemDefCurrentStacks, const UObsidianInventoryItemInstance* InstanceToAddTo);
	
	/** Checks the limit of the item, returns the number of stacks available to add to the inventory with provided ItemDef. */
	int32 GetNumberOfStacksAvailableToAddToInventory(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef, const int32 CurrentStacks);

	/** Checks the limit of the item, returns the number of stacks available to add to the inventory with provided instance. */
	int32 GetNumberOfStacksAvailableToAddToInventory(const UObsidianInventoryItemInstance* ItemInstance);

	/** Gets the item location from internal grid map. */
	FVector2D GetItemLocationFromGrid(UObsidianInventoryItemInstance* ItemInstance) const;
	
	/** Checks if the item fits in the inventory, outputs the first available position.  */
	bool CheckAvailablePosition(const TArray<FVector2D>& ItemGridSize, FVector2D& OutAvailablePosition);
	
	/** Internal usage only, this returns the internal Location To Instance Map. */
	TMap<FVector2D, UObsidianInventoryItemInstance*> Internal_GetLocationToInstanceMap();
	TMap<FVector2D, bool> Internal_GetInventoryStateMap();

	//TODO This can be moved to some library later.
	
	/** Will compare item's definitions, will return true if items are of the same class. */
	static bool IsTheSameItem(const UObsidianInventoryItemInstance* InstanceA, const UObsidianInventoryItemInstance* InstanceB);

	/** Will compare item's definitions, will return true if items are of the same class. */
	static bool IsTheSameItem(const UObsidianInventoryItemInstance* Instance, const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef);

	//END TODO
	
private:
	friend UObsidianInventoryWidgetController;

#if WITH_GAMEPLAY_DEBUGGER
	friend FGameplayDebuggerCategory_InventoryItems;
#endif
	
	/**
	 * Actual array of items which is FFastArraySerializer.
	 * It also contains Map which maps Grid Vector2D location to actual Item Instance in the inventory.
	 */
	UPROPERTY(Replicated)
	FObsidianInventoryGrid InventoryGrid;
	
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|InventorySetup")
	int32 InventoryGridWidth = 12;
	
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|InventorySetup")
	int32 InventoryGridHeight = 5;

	/** Grid size of the inventory, calculated (InventoryGridWidth * InventoryGridHeight). */
	int32 InventoryGridSize = 0;

	bool bIsLocallyControlled = false;
};
