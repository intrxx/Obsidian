// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "Components/ActorComponent.h"
#include "CoreMinimal.h"

#include "ObsidianTypes/ItemTypes/ObsidianItemTypes.h"

#include "ObsidianItemManagerComponent.generated.h"

class UObsidianDraggedItem;
class AObsidianDroppableItem;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnStartDraggingItemSignature, const FDraggedItem& DraggedItem)
DECLARE_MULTICAST_DELEGATE(FOnStopDraggingItemSignature)

/**
 * Component that manages various Item related actions. 
 */
UCLASS()
class OBSIDIAN_API UObsidianItemManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UObsidianItemManagerComponent(const FObjectInitializer& InObjectInitializer = FObjectInitializer::Get());

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void TickComponent(float InDeltaTime, ELevelTick InTickType,
		FActorComponentTickFunction* InThisTickFunction) override;

	FDraggedItem GetDraggedItem();
	bool IsDraggingAnItem() const;
	bool CanDropItem() const;

	bool DidJustDroppedItem() const;
	void SetJustDroppedItem(const bool bInJustDroppedItem);

	/**
	 * Inventory Items.
	 */
	
	bool DropItem();
	
	UFUNCTION(Server, Reliable)
	void ServerAddItemToInventoryAtSlot(const FIntPoint& InSlotPosition, const bool bInShiftDown);
	
	UFUNCTION(Server, Reliable)
	void ServerAddStacksFromDraggedItemToInventoryItemAtSlot(const FIntPoint& InSlotPosition,
		const int32 InStacksToAddOverride = -1);

	UFUNCTION(Server, Reliable)
	void ServerTakeoutFromInventoryItem(const FIntPoint& InSlotPosition, const int32 InStacksToTake);

	UFUNCTION(Server, Reliable)
	void ServerReplaceItemAtInventorySlot(const FIntPoint& InClickedItemPosition, const FIntPoint& InClickedGridPosition);
	
	UFUNCTION(Server, Reliable)
	void ServerGrabDroppableItemToCursor(AObsidianDroppableItem* InItemToPickup);
	
	UFUNCTION(Server, Reliable)
	void ServerGrabInventoryItemToCursor(const FIntPoint& InSlotPosition);
	
	UFUNCTION(Server, Reliable)
	void ServerPickupItem(AObsidianDroppableItem* InItemToPickup);

	UFUNCTION(Server, Reliable)
	void ServerTransferItemToPlayerStash(const FIntPoint& InFromInventoryPosition, const FGameplayTag& InToStashTab);
	
	/**
	 * Equipment.
	 */

	UFUNCTION(Server, Reliable)
	void ServerEquipItemAtSlot(const FGameplayTag& InSlotTag);

	UFUNCTION(Server, Reliable)
	void ServerGrabEquippedItemToCursor(const FGameplayTag& InSlotTag);
	
	UFUNCTION(Server, Reliable)
	void ServerReplaceItemAtEquipmentSlot(const FGameplayTag& InSlotTag,
		const FGameplayTag& InEquipSlotTagOverride = FGameplayTag::EmptyTag);

	UFUNCTION(Server, Reliable)
	void ServerWeaponSwap();

	/**
	 * Player Stash.
	 */

	UFUNCTION(Server, Reliable)
	void ServerAddItemToStashTabAtSlot(const FObsidianItemPosition& InAtPosition, const bool bInShiftDown);

	UFUNCTION(Server, Reliable)
	void ServerAddStacksFromDraggedItemToStashedItemAtSlot(const FObsidianItemPosition& InAtPosition,
		const int32 InStacksToAddOverride = -1);
	
	UFUNCTION(Server, Reliable)
	void ServerGrabStashedItemToCursor(const FObsidianItemPosition& InFromPosition);

	UFUNCTION(Server, Reliable)
	void ServerTransferItemToInventory(const FObsidianItemPosition& InFromStashPosition);

	UFUNCTION(Server, Reliable)
	void ServerReplaceItemAtStashPosition(const FObsidianItemPosition& InAtStashPosition);

	UFUNCTION(Server, Reliable)
	void ServerTakeoutFromStashedItem(const FObsidianItemPosition& InAtStashPosition, const int32 InStacksToTake);
	
	//~ Start of UObject interface
	virtual bool ReplicateSubobjects(UActorChannel* InChannel, FOutBunch* InBunch, FReplicationFlags* InRepFlags) override;
	virtual void ReadyForReplication() override;
	//~ End of UObject interface

public:
	FOnStartDraggingItemSignature OnStartDraggingItemDelegate;
	FOnStopDraggingItemSignature OnStopDraggingItemDelegate;

protected:
	UFUNCTION()
	void OnRep_DraggedItem(const FDraggedItem& InOldDraggedItem);
	
	void DragItem() const;
	
	void StartDraggingItem(const AController* InController);
	void StopDraggingItem(const AController* InController);

	void UpdateDraggedItem(const FObsidianItemOperationResult& InOperationResult, const int32 InCachedNumberOfStack,
		const AController* InForController);
	void UpdateDraggedItem(const FObsidianAddingStacksResult& InOperationResult, const int32 InCachedNumberOfStack,
		const AController* InForController);

	/**
	 * This is a very specific function that is used to determine if the dragged item was changed in a result of replacing it with
	 * another item in the inventory in OnRep_DraggedItem. If it sounds useful be careful when using it.
	 */
	bool DraggedItemWasReplaced(const FDraggedItem& InOldDraggedItem) const;
	void UpdateStacksOnDraggedItemWidget(const int32 InStacks);
	
	/**
	 * Item Drop.
	 */

	UFUNCTION(Server, Reliable)
	void ServerHandleDroppingItem();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obsidian|Items", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UObsidianDraggedItem> DraggedItemWidgetClass;
	
	/** Radius of the sphere in which we allow the Player to drop item. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Obsidian|Input")
	float DropRadius = 200.0f;

private:
	bool VerifyPickupRange(const AObsidianDroppableItem* InItemToPickUp) const;
	/** Stash actions are only allowed when the owning Player stands next to some Player Stash. */
	bool IsOwnerInPlayerStashRange() const;
	bool IsOwnerInInteractionRange(const AActor* InInteractionActor, const float InInteractionRadius) const;

private:

	/**
	 * Dragged items.
	 */
	UPROPERTY()
	TObjectPtr<UObsidianDraggedItem> DraggedItemWidget;
	
	UPROPERTY(ReplicatedUsing = OnRep_DraggedItem)
	FDraggedItem DraggedItem = FDraggedItem();
	
#if 0 // https://github.com/intrxx/Obsidian/commit/e3eda3899a1b39ec1952221a24bce0b40b7be769
	FVector CachedItemDropLocation = FVector::ZeroVector;
#endif

	bool bDraggingItem = false;
	bool bItemAvailableForDrop = false;
	bool bJustDroppedItem = false;
};
