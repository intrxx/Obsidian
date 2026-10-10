// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"

#include "InventoryItems/ObsidianItemContainerComponent.h"
#include "InventoryItems/PlayerStash/ObsidianStashItemList.h"

#include "ObsidianPlayerStashComponent.generated.h"

class UObsidianInventoryComponent;
struct FObsidianSavedItem;

class UObsidianStashTabsConfig;
class UObsidianInventoryItemDefinition;
class UObsidianInventoryItemInstance;
class AObsidianPlayerController;

/**
 * Primary Player Stash Component of Obsidian to be used by Players.
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class OBSIDIAN_API UObsidianPlayerStashComponent : public UObsidianItemContainerComponent
{
	GENERATED_BODY()

public:	
	UObsidianPlayerStashComponent(const FObjectInitializer& InObjectInitializer = FObjectInitializer::Get());
	
	/** Returns the Player Stash Component if one exists on the specified actor, will be nullptr otherwise */
	UFUNCTION(BlueprintPure, Category = "Obsidian|Inventory")
	static UObsidianPlayerStashComponent* FindPlayerStashComponent(const AActor* InActor)
	{
		return (InActor ? InActor->FindComponentByClass<UObsidianPlayerStashComponent>() : nullptr);
	}

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	TConstArrayView<TObjectPtr<UObsidianStashTab>> GetAllStashTabs() const;
	
	bool CanOwnerModifyPlayerStashState();
	
	TArray<UObsidianInventoryItemInstance*> GetAllItems() const;
	TArray<UObsidianInventoryItemInstance*> GetAllPersonalItems() const;
	TArray<UObsidianInventoryItemInstance*> GetAllSharedItems() const;
	TArray<UObsidianInventoryItemInstance*> GetAllItemsFromStashTab(const FGameplayTag& InStashTabTag);
	UObsidianInventoryItemInstance* GetItemInstanceFromTabAtPosition(const FObsidianItemPosition& InItemPosition);

	void InitializeStashTabs();

	TArray<FObsidianStashSlotDefinition> FindMatchingSlotsForItemCategory(const FGameplayTag& InItemCategory,
		const FGameplayTag& InItemBaseType);
	TArray<FObsidianStashSlotDefinition> FindPossibleSlotsForPlacingItem_WithInstance(const UObsidianInventoryItemInstance* InForInstance);
	TArray<FObsidianStashSlotDefinition> FindPossibleSlotsForPlacingItem_WithItemDef(const TSubclassOf<UObsidianInventoryItemDefinition>& InForItemDef);

	UObsidianStashTab* GetStashTabForTag(const FGameplayTag& InStashTabTag);
	
	/** Finds all stacks in the inventory for given item type with item Def. */
	int32 FindAllStacksForGivenItem(const TSubclassOf<UObsidianInventoryItemDefinition>& InItemDef);
	
	/** Finds all stacks in the inventory for given item type with item Instance. */
	int32 FindAllStacksForGivenItem(const UObsidianInventoryItemInstance* InItemInstance);

	/** Checks if the item fits in the provided spot. */
	bool CheckSpecifiedPosition(const FObsidianItemPosition& InSpecifiedPosition, const FGameplayTag& InItemCategory,
		const FGameplayTag& InItemBaseTypeTag, const FIntPoint& InItemGridSpan);

	bool CanFitInstanceInStashTab(const FIntPoint& InItemGridSpan, const FGameplayTag& InItemCategory, const FGameplayTag& InItemBaseTypeTag,
		const FGameplayTag& InStashTabTag);
	
	bool CanReplaceItemAtPosition(const FObsidianItemPosition& InAtItemPosition, const UObsidianInventoryItemInstance* InReplacingInstance);
	bool CanReplaceItemAtPosition(const FObsidianItemPosition& InAtItemPosition, const TSubclassOf<UObsidianInventoryItemDefinition>& InReplacingDef);
	
	/**
	 * Will try to add provided amount of stacks of provided Item to any of the same Item present in the Inventory. Returns Array of Instances that stacks were added to.
	 *
	 *	@param AddingFromItemDef		The Item Definition that the function will try to add from.
	 *	@param StacksToAdd				The Current Stacks of the provided Item Definition.
	 *	@param InTabTag					Stash Tab Tag which contains the item.
	 *  @param OutAddedToInstances		Array of Inventory Item Instances that stacks were added to.
	 *
	 *  @return The struct that contains various useful information about the result of the adding process.
	 */
	FObsidianAddingStacksResult TryAddingStacksToExistingItems(const TSubclassOf<UObsidianInventoryItemDefinition>& InAddingFromItemDef,
		const int32 InStacksToAdd, const FGameplayTag& InTabTag, TArray<UObsidianInventoryItemInstance*>& OutAddedToInstances);
	
	/** Tries to add Item Definition to the Opened Stash Tab, if the item is stackable will first try to add all the stacks to the same item types if they exist in the Stash Tab. */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|PlayerStash")
	FObsidianItemOperationResult AddItemDefinition(const TSubclassOf<UObsidianInventoryItemDefinition> InItemDef,
		const FGameplayTag& InStashTabTag, const FObsidianItemGeneratedData& InItemGeneratedData);

	/** Tries to add provided Item Definition to provided Slot. */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|PlayerStash")
	FObsidianItemOperationResult AddItemDefinitionToSpecifiedSlot(const TSubclassOf<UObsidianInventoryItemDefinition> InItemDef,
		const FObsidianItemPosition& InItemPosition, const FObsidianItemGeneratedData& InItemGeneratedData, const int32 InStackToAddOverride = -1);

	/**
	 *	Will try to add stacks from provided Item Definition at provided Position.
	 *
	 *	@param AddingFromItemDef				The Item Definition that the function will try to add from.
	 *	@param AddingFromItemDefCurrentStacks	The Current Stacks of the provided Item Definition.
	 *  @param AtPosition						The clicked Stash location at which the function will search for item to add to.		
	 *  @param StackToAddOverride				Optional override for the amount of stacks the function will try to add to the item. Will do nothing if unused.
	 *
	 *  @return The struct that contains various useful information about the result of the adding process.
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|Inventory")
	FObsidianAddingStacksResult TryAddingStacksToSpecificSlotWithItemDef(const TSubclassOf<UObsidianInventoryItemDefinition>& InAddingFromItemDef,
		const int32 InAddingFromItemDefCurrentStacks, const FObsidianItemPosition& InAtPosition, const int32 InStackToAddOverride = -1);
	
	/** Tries to add Item Instance to the Player Stash, if the item is stackable will first try to add all the stacks to the same item types if they exist in the Stash Tab. */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|PlayerStash")
	FObsidianItemOperationResult AddItemInstance(UObsidianInventoryItemInstance* InInstanceToAdd, const FGameplayTag& InStashTabTag);
	
	/** Tries to add provided Item Instance to provided Slot. */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|PlayerStash")
	FObsidianItemOperationResult AddItemInstanceToSpecificSlot(UObsidianInventoryItemInstance* InInstanceToAdd,
		const FObsidianItemPosition& InItemPosition, const int32 InStackToAddOverride = -1);

	/**
	 *	Will try to add stacks from provided Item Instance at provided Position. 
	 *
	 *	@param AddingFromInstance		The Item Instance that the function will try to add from.
	 *  @param AtPosition				The clicked Stash location at which the function will search for item to add to.
	 *  @param StackToAddOverride		Optional override for the amount of stacks the function will try to add to the item. Will do nothing if unused.
	 *
	 *  @return The struct that contains various useful information about the result of the adding process.
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|PlayerStash")
	FObsidianAddingStacksResult TryAddingStacksToSpecificSlotWithInstance(UObsidianInventoryItemInstance* InAddingFromInstance,
		const FObsidianItemPosition& InAtPosition, const int32 InStackToAddOverride = -1);

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
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|PlayerStash")
	FObsidianItemOperationResult RemoveItemInstance(UObsidianInventoryItemInstance* InInstanceToRemove);


	
	UFUNCTION(Server, Reliable)
	void ServerRegisterAndValidateCurrentStashTab(const FGameplayTag& InStashTab);
	FGameplayTag GetActiveStashTag() const;

	void LoadStashedItem(const FObsidianSavedItem& InStashedSavedItem);

	//~ Start of UObsidianItemContainerComponent interface
	virtual TArray<UObsidianInventoryItemInstance*> GetContainedItems() const override;
	//~ End of UObsidianItemContainerComponent interface

protected:
	/** Checks if the provided Item Definition fits anywhere in the Stash Tab (for tag). Provides Available Position. */
	bool CanFitItemDefinition(FObsidianItemPosition& OutAvailablePosition, const FGameplayTag& InStashTabTag,
		const TSubclassOf<UObsidianInventoryItemDefinition>& InItemDef);

	/** Checks if the provided Item Definition fits in the Stash Tab (for tag) at provided slot. */
	bool CanFitItemDefinitionToSpecifiedSlot(const FObsidianItemPosition& InSpecifiedSlot,
		const TSubclassOf<UObsidianInventoryItemDefinition>& InItemDef);

	/** Checks if the item fits in the inventory, outputs the first available position.  */
	bool CheckAvailablePosition(FObsidianItemPosition& OutAvailablePosition, const FIntPoint& InItemGridSpan,
		const FGameplayTag& InItemCategory, const FGameplayTag& InItemBaseTypeTag, const FGameplayTag& InStashTabTag);

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
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	TObjectPtr<UObsidianStashTabsConfig> StashTabsConfig;



	/** This is not very clean I think, but I need to store Stash Tabs in some UPROPERTY container so the GC won't get it :/ */
	UPROPERTY()
	TArray<TObjectPtr<UObsidianStashTab>> StashTabs;

private:
	
#if WITH_GAMEPLAY_DEBUGGER
	friend class FGameplayDebuggerCategory_PlayerStash;
#endif
	
	/**
	 * Actual array of stashed items which is FFastArraySerializer.
	 */
	UPROPERTY(Replicated)
	FObsidianStashItemList StashItemList;

	UPROPERTY(Replicated)
	FGameplayTag CurrentStashTab = FGameplayTag::EmptyTag;
};
