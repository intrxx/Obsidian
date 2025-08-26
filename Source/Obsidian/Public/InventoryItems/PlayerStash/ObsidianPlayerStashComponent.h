// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project
#include "InventoryItems/PlayerStash/ObsidianStashItemList.h"

#include "Components/ActorComponent.h"
#include "ObsidianPlayerStashComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogPlayerStash, Display, All);

class UObsidianStashTabsConfig;
class UObsidianInventoryItemDefinition;
class UObsidianInventoryItemInstance;
class AObsidianPlayerController;

/**
 * Primary Player Stash Component of Obsidian to be used by Players.
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class OBSIDIAN_API UObsidianPlayerStashComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UObsidianPlayerStashComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	/** Returns the Player Stash Component if one exists on the specified actor, will be nullptr otherwise */
	UFUNCTION(BlueprintPure, Category = "Obsidian|Inventory")
	static UObsidianPlayerStashComponent* FindPlayerStashComponent(const AActor* Actor)
	{
		return (Actor ? Actor->FindComponentByClass<UObsidianPlayerStashComponent>() : nullptr);
	}

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UObsidianStashTabsConfig* GetStashTabConfig() const;
	
	bool CanOwnerModifyPlayerStashState();
	
	TArray<UObsidianInventoryItemInstance*> GetAllItems() const;
	TArray<UObsidianInventoryItemInstance*> GetAllItemsFromStashTab(const FGameplayTag& StashTabTag);
	UObsidianInventoryItemInstance* GetInstanceFromTabAtPosition(const FObsidianItemPosition& ItemPosition);

	UObsidianStashTab* GetStashTabForTag(const FGameplayTag& StashTabTag);
	
	/** Finds all stacks in the inventory for given item type with item Def. */
	int32 FindAllStacksForGivenItem(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef);
	
	/** Finds all stacks in the inventory for given item type with item Instance. */
	int32 FindAllStacksForGivenItem(const UObsidianInventoryItemInstance* ItemInstance);

	/** Checks if the item fits in the provided spot. */
	bool CheckSpecifiedPosition(const FIntPoint& ItemGridSpan, const FGameplayTag& ItemCategory, const FObsidianItemPosition& SpecifiedPosition);

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
	FObsidianAddingStacksResult TryAddingStacksToExistingItems(const TSubclassOf<UObsidianInventoryItemDefinition>& AddingFromItemDef, const int32 StacksToAdd, const FGameplayTag& InTabTag, TArray<UObsidianInventoryItemInstance*>& OutAddedToInstances);

	/** Tries to add Item Definition to the Opened Stash Tab, if the item is stackable will first try to add all the stacks to the same item types if they exist in the Stash Tab. */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|Inventory")
	FObsidianItemOperationResult AddItemDefinition(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef, const FGameplayTag& StashTabTag, const int32 StackCount = 1);

	/** Tries to add provided Item Definition to provided Slot. */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|PlayerStash")
	FObsidianItemOperationResult AddItemDefinitionToSpecifiedSlot(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef, const FObsidianItemPosition& ItemPosition, const int32 StackCount = 1, const int32 StackToAddOverride = -1);

	/** Tries to add Item Instance to the Player Stash, if the item is stackable will first try to add all the stacks to the same item types if they exist in the Stash Tab. */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|Inventory")
	FObsidianItemOperationResult AddItemInstance(UObsidianInventoryItemInstance* InstanceToAdd, const FGameplayTag& StashTabTag);
	
	/** Tries to add provided Item Instance to provided Slot. */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|Inventory")
	FObsidianItemOperationResult AddItemInstanceToSpecificSlot(UObsidianInventoryItemInstance* InstanceToAdd, const FObsidianItemPosition& ItemPosition, const int32 StackToAddOverride = -1);

	/** Removes Item Instance from inventory. */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Obsidian|Inventory")
	FObsidianItemOperationResult RemoveItemInstance(UObsidianInventoryItemInstance* InstanceToRemove, const FGameplayTag& StashTabTag);

	//~ Start of UObject interface
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	virtual void ReadyForReplication() override;
	//~ End of UObject interface

protected:
	virtual void BeginPlay() override;
	
	/** Checks if the provided Item Definition fits anywhere in the Stash Tab (for tag). Provides Available Position. */
	bool CanFitItemDefinition(FObsidianItemPosition& OutAvailablePosition, const FGameplayTag& StashTabTag, const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef);

	/** Checks if the provided Item Definition fits in the Stash Tab (for tag) at provided slot. */
	bool CanFitItemDefinitionToSpecifiedSlot(const FObsidianItemPosition& SpecifiedSlot, const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef);

	/** Checks if the item fits in the inventory, outputs the first available position.  */
	bool CheckAvailablePosition(FObsidianItemPosition& OutAvailablePosition, const FIntPoint& ItemGridSpan, const FGameplayTag& ItemCategory, const FGameplayTag& StashTabTag);
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian")
	TObjectPtr<UObsidianStashTabsConfig> StashTabsConfig;

	UPROPERTY()
	TObjectPtr<AObsidianPlayerController> CachedOwnerPlayerController;

private:
	
#if WITH_GAMEPLAY_DEBUGGER
	friend class FGameplayDebuggerCategory_PlayerStash;
#endif
	
	/**
	 * Actual array of stashed items which is FFastArraySerializer.
	 */
	UPROPERTY(Replicated)
	FObsidianStashItemList StashItemList;
};
