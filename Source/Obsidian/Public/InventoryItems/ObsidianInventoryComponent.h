// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "ObsidianInventoryGrid.h"
#include "Components/ActorComponent.h"
#include "ObsidianInventoryComponent.generated.h"

class UObsidianInventoryWidgetController;

DECLARE_LOG_CATEGORY_EXTERN(LogInventory, Log, All);

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnItemAddedToInventorySignature, UObsidianInventoryItemInstance* ItemInstance, FVector2D DesiredPosition);

using FLocationToStacksMap = const TMap<FVector2D, int32>&; // Need to wrap the Map in type alias to avoid parsing issues in delegate macro.
DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemsStacksChangedSignature, FLocationToStacksMap);

/**
 * Primary Inventory Component of Obsidian to be used by Characters.
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
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
	
	int32 GetTotalItemCountByDefinition(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef) const;
	
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Obsidian|Inventory")
	TArray<UObsidianInventoryItemInstance*> GetAllItems() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Obsidian|Inventory")
	UObsidianInventoryItemInstance* FindFirstItemStackForDefinition(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef) const;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|Inventory")
	bool CanAddItemDefinition(FVector2D& OutAvailablePosition, const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef, int32 StackCount = 1);
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|Inventory")
	bool CanAddItemDefinitionToSpecifiedSlot(const FVector2D& SpecifiedSlot, const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef, int32 StackCount = 1);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|Inventory")
	bool CanReplaceItemAtSpecificSlotWithDef(const FVector2D& Slot, const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef, const int32 StackCount);
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|Inventory")
	UObsidianInventoryItemInstance* AddItemDefinition(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef, int32& OutStacksLeft, const int32 StackCount = 1);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|Inventory")
	UObsidianInventoryItemInstance* AddItemDefinitionToSpecifiedSlot(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef, const FVector2D& ToSlot, const int32 StackCount = 1);
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|Inventory")
	bool CanAddItemInstance(FVector2D& OutAvailablePosition, UObsidianInventoryItemInstance* Instance);
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|Inventory")
	bool CanAddItemInstanceToSpecificSlot(const FVector2D& SpecifiedSlot, UObsidianInventoryItemInstance* Instance);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|Inventory")
	bool CanReplaceItemAtSpecificSlotWithInstance(const FVector2D& Slot, UObsidianInventoryItemInstance* ReplacingInstance);
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|Inventory")
	void AddItemInstance(UObsidianInventoryItemInstance* InstanceToAdd, int32& OutStacksLeft);
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|Inventory")
	bool AddItemInstanceToSpecificSlot(UObsidianInventoryItemInstance* InstanceToAdd, const FVector2D& ToSlot);
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|Inventory")
	void RemoveItemInstance(UObsidianInventoryItemInstance* InstanceToRemove);

	bool ConsumeItemsByDefinition(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef, const int32 NumberOfItemsToConsume);
	
	//~ Start of UObject interface
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	virtual void ReadyForReplication() override;
	//~ End of UObject interface

public:
	FOnItemAddedToInventorySignature OnItemAddedToInventoryDelegate;
	FOnItemsStacksChangedSignature OnItemsStacksChangedDelegate;

private:
	void InitInventoryState();
	
	TArray<UObsidianInventoryItemInstance*> TryAddingStacksToExistingItem(const TSubclassOf<UObsidianInventoryItemDefinition>& NewItemDef, const int32 NewItemStacks, int32& OutStacksLeft);
	UObsidianInventoryItemInstance* TryAddingStacksToSpecificSlotWithItemDef(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef, const FVector2D& AtPosition, const int32 NewItemStacks, int32& OutStacksLeft, int32& OutStacksAdded);

	/** Will try to add stacks from provided Item Instance at specific slot, will return false if there is no matching item AtPosition or entire stack could not be added. */
	bool TryAddingStacksToSpecificSlotWithInstance(UObsidianInventoryItemInstance* NewItemInstance, const FVector2D& AtPosition, int32& OutStacksLeft, int32& OutStacksAdded);

	/** Finds all stacks in the inventory for given item type with item Def. */
	int32 FindAllStacksForGivenItem(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef);
	
	/** Finds all stacks in the inventory for given item type with item Instance. */
	int32 FindAllStacksForGivenItem(const UObsidianInventoryItemInstance* ItemInstance);

	/** Checks the limit of the item, returns the number of stacks available to add with provided ItemDef. */
	int32 GetNumberOfStacksAvailableToAdd(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef, const int32 CurrentStacks);

	/** Checks the limit of the item, returns the number of stacks available to add with provided instance. */
	int32 GetNumberOfStacksAvailableToAdd(const UObsidianInventoryItemInstance* ItemInstance);
	
	FVector2D GetItemLocationFromGrid(UObsidianInventoryItemInstance* ItemInstance) const;
	
	void Item_MarkSpace(const UObsidianInventoryItemInstance* ItemInstance, const FVector2D AtPosition);
	void Item_UnMarkSpace(const UObsidianInventoryItemInstance* ItemInstance, const FVector2D AtPosition);

	/** Checks if the item fits in the inventory, outputs the first available position.  */
	bool CheckAvailablePosition(const TArray<FVector2D>& ItemGridSize, FVector2D& OutAvailablePosition);

	/** Checks if the item fits in the provided spot. */
	bool CheckSpecifiedPosition(const TArray<FVector2D>& ItemGridSize, const FVector2D& SpecifiedPosition);

	/** Internal usage only, this returns the internal Location To Instance Map. */
	TMap<FVector2D, UObsidianInventoryItemInstance*> Internal_GetLocationToInstanceMap();
	TMap<FVector2D, bool> Internal_GetInventoryStateMap();
	UObsidianInventoryItemInstance* Internal_GetItemInstanceAtLocation(const FVector2D& Position) const;
	
private:
	friend UObsidianInventoryWidgetController;
	
	UPROPERTY(Replicated)
	FObsidianInventoryGrid InventoryGrid;

	UPROPERTY()
	TMap<FVector2D, bool> InventoryStateMap;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|InventorySetup")
	int32 InventoryGridWidth = 12;
	
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|InventorySetup")
	int32 InventoryGridHeight = 5;
	
	int32 InventoryGridSize = 0;
};
