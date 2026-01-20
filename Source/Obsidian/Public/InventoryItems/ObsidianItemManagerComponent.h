// Copyright 2026 out of sCope team - intrxx

#pragma once

#include <CoreMinimal.h>

#include "ObsidianTypes/ItemTypes/ObsidianItemTypes.h"

#include <Components/ActorComponent.h>
#include "ObsidianItemManagerComponent.generated.h"

class UObsidianDraggedItem;
class AObsidianDroppableItem;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnStartDraggingItemSignature, const FDraggedItem& DraggedItem)
DECLARE_MULTICAST_DELEGATE(FOnStopDraggingItemSignature)

DECLARE_LOG_CATEGORY_EXTERN(LogItemManager, Log, All);

/**
 * Component that manages various Item related actions. 
 */
UCLASS()
class OBSIDIAN_API UObsidianItemManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UObsidianItemManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;

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
	void ServerAddItemToInventoryAtSlot(const FIntPoint& SlotPosition, const bool bShiftDown);
	
	UFUNCTION(Server, Reliable)
	void ServerAddStacksFromDraggedItemToInventoryItemAtSlot(const FIntPoint& SlotPosition,
		const int32 StacksToAddOverride = -1);

	UFUNCTION(Server, Reliable)
	void ServerTakeoutFromInventoryItem(const FIntPoint& SlotPosition, const int32 StacksToTake);

	UFUNCTION(Server, Reliable)
	void ServerReplaceItemAtInventorySlot(const FIntPoint& ClickedItemPosition, const FIntPoint& ClickedGridPosition);
	
	UFUNCTION(Server, Reliable)
	void ServerGrabDroppableItemToCursor(AObsidianDroppableItem* ItemToPickup);
	
	UFUNCTION(Server, Reliable)
	void ServerGrabInventoryItemToCursor(const FIntPoint& SlotPosition);
	
	UFUNCTION(Server, Reliable)
	void ServerPickupItem(AObsidianDroppableItem* ItemToPickup);

	UFUNCTION(Server, Reliable)
	void ServerTransferItemToPlayerStash(const FIntPoint& FromInventoryPosition, const FGameplayTag& ToStashTab);
	
	/**
	 * Equipment.
	 */

	UFUNCTION(Server, Reliable)
	void ServerEquipItemAtSlot(const FGameplayTag& SlotTag);

	UFUNCTION(Server, Reliable)
	void ServerGrabEquippedItemToCursor(const FGameplayTag& SlotTag);
	
	UFUNCTION(Server, Reliable)
	void ServerReplaceItemAtEquipmentSlot(const FGameplayTag& SlotTag,
		const FGameplayTag& EquipSlotTagOverride = FGameplayTag::EmptyTag);

	UFUNCTION(Server, Reliable)
	void ServerWeaponSwap();

	/**
	 * Player Stash.
	 */

	UFUNCTION(Server, Reliable)
	void ServerAddItemToStashTabAtSlot(const FObsidianItemPosition& AtPosition, const bool bShiftDown);

	UFUNCTION(Server, Reliable)
	void ServerAddStacksFromDraggedItemToStashedItemAtSlot(const FObsidianItemPosition& AtPosition,
		const int32 StacksToAddOverride = -1);
	
	UFUNCTION(Server, Reliable)
	void ServerGrabStashedItemToCursor(const FObsidianItemPosition& FromPosition);

	UFUNCTION(Server, Reliable)
	void ServerTransferItemToInventory(const FObsidianItemPosition& FromStashPosition);

	UFUNCTION(Server, Reliable)
	void ServerReplaceItemAtStashPosition(const FObsidianItemPosition& AtStashPosition);

	UFUNCTION(Server, Reliable)
	void ServerTakeoutFromStashedItem(const FObsidianItemPosition& AtStashPosition, const int32 StacksToTake);
	
	//~ Start of UObject interface
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	virtual void ReadyForReplication() override;
	//~ End of UObject interface

public:
	FOnStartDraggingItemSignature OnStartDraggingItemDelegate;
	FOnStopDraggingItemSignature OnStopDraggingItemDelegate;

protected:
	UFUNCTION()
	void OnRep_DraggedItem(const FDraggedItem& OldDraggedItem);
	
	void DragItem() const;
	
	void StartDraggingItem(const AController* Controller);
	void StopDraggingItem(const AController* Controller);

	void UpdateDraggedItem(const FObsidianItemOperationResult& OperationResult, const int32 CachedNumberOfStack,
		const AController* ForController);
	void UpdateDraggedItem(const FObsidianAddingStacksResult& OperationResult, const int32 CachedNumberOfStack,
		const AController* ForController);

	/**
	 * This is a very specific function that is used to determine if the dragged item was changed in a result of replacing it with
	 * another item in the inventory in OnRep_DraggedItem. If it sounds useful be careful when using it.
	 */
	bool DraggedItemWasReplaced(const FDraggedItem& OldDraggedItem) const;
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
	bool VerifyPickupRange(const AObsidianDroppableItem* ItemToPickUp);

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
