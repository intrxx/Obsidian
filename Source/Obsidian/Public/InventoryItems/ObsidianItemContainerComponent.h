// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "Components/ActorComponent.h"
#include "CoreMinimal.h"

#include "ObsidianTypes/ItemTypes/ObsidianItemTypes.h"

#include "ObsidianItemContainerComponent.generated.h"

class AObsidianPlayerController;
class AObsidianPlayerState;
class UObsidianAbilitySystemComponent;
class UObsidianInventoryComponent;
class UObsidianEquipmentComponent;
class UObsidianPlayerStashComponent;
class UObsidianInventoryItemInstance;
class UObsidianInventoryItemDefinition;

/**
 * 
 */
UCLASS(Abstract)
class OBSIDIAN_API UObsidianItemContainerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UObsidianItemContainerComponent(const FObjectInitializer& InObjectInitializer = FObjectInitializer::Get());

	AObsidianPlayerController* GetOwnerPlayerController() const;
	AObsidianPlayerState* GetObsidianPlayerStateFromOwner() const;
	UObsidianAbilitySystemComponent* GetObsidianAbilitySystemComponentFromOwner() const;
	UObsidianInventoryComponent* GetInventoryComponentFromOwner() const;
	UObsidianEquipmentComponent* GetEquipmentComponentFromOwner() const;
	UObsidianPlayerStashComponent* GetStashComponentFromOwner() const;

	/** Gets the Owner's container that holds items at given position, might be nullptr. */
	UObsidianItemContainerComponent* GetContainerFromOwnerForPosition(const FObsidianItemPosition& InItemPosition) const;

	/** Checks if the Owner is currently allowed to change the state of this container, see GetBlockActionsTag. */
	bool CanOwnerModifyContainerState() const;

	/** Gets every Item Instance held by this container. */
	virtual TArray<UObsidianInventoryItemInstance*> GetContainedItems() const PURE_VIRTUAL(UObsidianItemContainerComponent::GetContainedItems, return {};);

	/** Sums up current stacks of every item of given Item Definition held by this container. */
	int32 CountStacksOfItem(const TSubclassOf<UObsidianInventoryItemDefinition>& InItemDef) const;

	/** Firing the OnUse functionality of passed UsingInstance onto UsingOntoInstance. */
	void UseItem(UObsidianInventoryItemInstance* InUsingInstance, UObsidianInventoryItemInstance* InUsingOntoInstance = nullptr);

	/** Updates the state of using item after it was used, the item can be held by any of the Owner's containers. */
	void UpdateUsingItemAfterUsage(UObsidianInventoryItemInstance* InUsingInstance, const int32 InCurrentStacks);

	//~ Start of UObject interface
	virtual bool ReplicateSubobjects(UActorChannel* InChannel, FOutBunch* InBunch, FReplicationFlags* InRepFlags) override;
	virtual void ReadyForReplication() override;
	//~ End of UObject interface

protected:
	/**
	 * Container specifics, implemented by derived classes.
	 */

	/** Gameplay Tag that blocks any modification of this container when present on the Owner's Ability System Component. */
	virtual FGameplayTag GetBlockActionsTag() const PURE_VIRTUAL(UObsidianItemContainerComponent::GetBlockActionsTag, return FGameplayTag::EmptyTag;);

	/** Adds already existing Item Instance to the underlying item list at given position. */
	virtual void AddItemInstanceToList(UObsidianInventoryItemInstance* InInstance, const FObsidianItemPosition& InToPosition) {}

	/** Creates new Item Instance from Item Definition in the underlying item list at given position, might return nullptr. */
	virtual UObsidianInventoryItemInstance* AddItemDefinitionToList(const TSubclassOf<UObsidianInventoryItemDefinition>& InItemDef,
		const FObsidianItemGeneratedData& InItemGeneratedData, const int32 InStackCount, const FObsidianItemPosition& InToPosition) { return nullptr; }

	/** Removes Item Instance from the underlying item list. */
	virtual void RemoveItemInstanceFromList(UObsidianInventoryItemInstance* InInstance) {}

	/** Called after the stack count of Item Instance held by this container changed. */
	virtual void HandleItemStacksChanged(UObsidianInventoryItemInstance* InInstance, const int32 InOldStackCount) {}

	/** Called after Item Instance held by this container changed in a way other than its stack count. */
	virtual void HandleItemChanged(UObsidianInventoryItemInstance* InInstance) {}

	/**
	 * Shared building blocks for derived classes.
	 */

	/** Checks if the Owner has authority, logs a warning with the calling function if it does not. */
	bool HasOwnerAuthority(const ANSICHAR* InCallingFunction) const;

	void RegisterItemInstanceForReplication(UObsidianInventoryItemInstance* InInstance);
	void UnregisterItemInstanceFromReplication(UObsidianInventoryItemInstance* InInstance);

	/** Applies the optional override to the number of stacks to add, INDEX_NONE means there is no override. */
	static int32 ClampStacksToAdd(const int32 InStacksAvailableToAdd, const int32 InStackToAddOverride);

	/** Creates new Item Instance from Item Definition at given position with given number of stacks, might return nullptr. */
	UObsidianInventoryItemInstance* PlaceItemDefinition(const TSubclassOf<UObsidianInventoryItemDefinition>& InItemDef,
		const FObsidianItemGeneratedData& InItemGeneratedData, const int32 InStackCount, const FObsidianItemPosition& InToPosition);

	/** Places the whole Item Instance at given position. */
	void PlaceWholeItemInstance(UObsidianInventoryItemInstance* InInstanceToAdd, const FObsidianItemPosition& InToPosition);

	/**
	 * Places given number of stacks from Item Instance at given position.
	 * If it is not the whole item, the stacks are split into a new Item Instance and the rest stays on the provided one.
	 *
	 * @return Item Instance that ended up in the container, either the provided one or the newly created one.
	 */
	UObsidianInventoryItemInstance* PlaceItemInstance(UObsidianInventoryItemInstance* InInstanceToAdd, const int32 InStacksToAdd,
		const FObsidianItemPosition& InToPosition, bool& bOutWholeItemPlaced);

	/** Tries to move stacks from Item Instance that is outside this container onto Item Instance held by this container. */
	FObsidianAddingStacksResult AddStacksToItemFromInstance(UObsidianInventoryItemInstance* InAddingFromInstance,
		UObsidianInventoryItemInstance* InInstanceToAddTo, const int32 InStackToAddOverride);

	/** Tries to add stacks from Item Definition onto Item Instance held by this container. */
	FObsidianAddingStacksResult AddStacksToItemFromDefinition(const TSubclassOf<UObsidianInventoryItemDefinition>& InAddingFromItemDef,
		const int32 InAddingFromItemDefCurrentStacks, UObsidianInventoryItemInstance* InInstanceToAddTo, const int32 InStackToAddOverride);

	/** Takes stacks out of Item Instance held by this container, the only valid number to take is in range [1, CurrentStacks - 1]. */
	FObsidianItemOperationResult TakeOutStacksFromItem(UObsidianInventoryItemInstance* InTakingFromInstance, const int32 InStacksToTake);

	/** Removes Item Instance from this container. */
	FObsidianItemOperationResult RemoveItemFromContainer(UObsidianInventoryItemInstance* InInstanceToRemove);

	/** Takes one stack from the used item held by this container, removes the item when it was the last one. */
	void ConsumeUsedItem(UObsidianInventoryItemInstance* InUsingInstance, const int32 InCurrentStacks);
};
