// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/PawnComponent.h"

// ~ Project
#include "ObsidianTypes/ObsidianItemTypes.h"

#include "ObsidianPlayerInputManager.generated.h"

struct FInputActionValue;

class UImage;
class UObsidianItem;
class UObsidianDraggedItem_Simple;
class AObsidianPlayerController;
class UObsidianInventoryItemInstance;
class AObsidianDroppableItem;
class USplineComponent;
class AObsidianHUD;
class IObsidianHighlightInterface;
class UObsidianDraggedItem;
class UObsidianInventoryItemDefinition;
class IObsidianInteractionInterface;

DECLARE_MULTICAST_DELEGATE(FOnStopUsingItemSignature)
DECLARE_MULTICAST_DELEGATE(FOnArrivedAtAcceptableItemPickupRangeSignature)
DECLARE_MULTICAST_DELEGATE(FOnArrivedAtAcceptableInteractionRangeSignature)

DECLARE_LOG_CATEGORY_EXTERN(LogInteraction, Log, All);

UENUM()
enum class EObsidianItemPickUpType : uint8
{
	/** Automatic pickup, the item goes straight to the inventory. */
	AutomaticPickUp,

	/** The item goes to Player cursor and is being dragged. */
	PickUpToDrag
};

/**
 * Component that manages every type of Player Input in obsidian. This includes regular native input, interactions,
 * highlights and Inventory Items handling.
 */
UCLASS()
class OBSIDIAN_API UObsidianPlayerInputManager : public UPawnComponent
{
	GENERATED_BODY()
	
public:
	UObsidianPlayerInputManager(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void InitializePlayerInput(UInputComponent* InputComponent);
	
	/** Returns the player input manager component if one exists on the specified actor. */
	UFUNCTION(BlueprintPure, Category = "Obsidian|PlayerInputManager")
	static UObsidianPlayerInputManager* FindPlayerInputManager(const AActor* Actor)
	{
		return (Actor ? Actor->FindComponentByClass<UObsidianPlayerInputManager>() : nullptr);
	}
	
	AObsidianHUD* GetObsidianHUD() const;
	
	FDraggedItem GetDraggedItem()
	{
		return DraggedItem;
	}

	UObsidianInventoryItemInstance* GetUsingItem()
	{
		return UsingItemInstance;
	}
	
	bool IsDraggingAnItem() const
	{
		return bDraggingItem;
	}

	bool IsUsingItem() const
	{
		return bUsingItem;
	}
	
	void SetUsingItem(const bool InbUsingItem, UObsidianItem* ItemWidget = nullptr, UObsidianInventoryItemInstance* UsingInstance = nullptr);
	
	/**
	 * Inventory Items.
	 */
	
	bool DropItem();
	
	UFUNCTION(Server, Reliable)
	void ServerAddItemToInventoryAtSlot(const FIntPoint& SlotPosition, const bool bShiftDown);
	
	UFUNCTION(Server, Reliable)
	void ServerAddStacksFromDraggedItemToItemAtSlot(const FIntPoint& SlotPosition, const int32 StacksToAddOverride = -1);

	UFUNCTION(Server, Reliable)
	void ServerTakeoutFromItem(const FIntPoint& SlotPosition, const int32 StacksToTake);

	UFUNCTION(Server, Reliable)
	void ServerReplaceItemAtInventorySlot(const FIntPoint& SlotPosition);
	
	UFUNCTION(Server, Reliable)
	void ServerGrabDroppableItemToCursor(AObsidianDroppableItem* ItemToPickup);
	
	UFUNCTION(Server, Reliable)
	void ServerGrabInventoryItemToCursor(const FIntPoint& SlotPosition);
	
	UFUNCTION(Server, Reliable)
	void ServerPickupItem(AObsidianDroppableItem* ItemToPickup);
	
	void UseItem(const FIntPoint& OnSlotPosition, const bool bLeftShiftDown);

	UFUNCTION(Server, Reliable)
	void ServerActivateUsableItem(UObsidianInventoryItemInstance* UsingInstance);
	
	/**
	 * Equipment.
	 */

	UFUNCTION(Server, Reliable)
	void ServerEquipItemAtSlot(const FGameplayTag& SlotTag);

	UFUNCTION(Server, Reliable)
	void ServerGrabEquippedItemToCursor(const FGameplayTag& SlotTag);
	
	UFUNCTION(Server, Reliable)
	void ServerReplaceItemAtEquipmentSlot(const FGameplayTag& SlotTag, const FGameplayTag& EquipSlotTagOverride = FGameplayTag::EmptyTag);

	UFUNCTION(Server, Reliable)
	void ServerWeaponSwap();

	//~ Start of UObject interface
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	virtual void ReadyForReplication() override;
	//~ End of UObject interface

public:
	FOnStopUsingItemSignature OnStopUsingItemDelegate;
	
protected:
	UFUNCTION()
	void OnRep_DraggedItem(const FDraggedItem& OldDraggedItem);
	
	void Input_AbilityInputTagPressed(FGameplayTag InputTag);
	void Input_AbilityInputTagReleased(FGameplayTag InputTag);
	
	void Input_MoveKeyboard(const FInputActionValue& InputActionValue);
	void Input_MoveStartedMouse();
	void Input_MoveTriggeredMouse();
	void Input_MoveReleasedMouse();
	
	void Input_ToggleCharacterStatus();
	void Input_ToggleInventory();
	void Input_TogglePassiveSkillTree();
	
	void Input_DropItem();
	void Input_ReleaseUsingItem();
	
	void Input_Interact();

	void Input_WeaponSwap();
	
protected:
	/** Time Threshold to know if it was a short press */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Obsidian|Input")
	float ShortPressThreshold = 0.3f;

	/** Acceptable radius to the destination of autorun */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Obsidian|Input")
	float AutoRunAcceptanceRadius = 30.f;

	/** Radius of the sphere in which we allow the Player to interact with the world e.g. open chest, pickup item etc. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Obsidian|Input")
	float DefaultInteractionRadius = 200.0f;

	/** Radius of the sphere in which we allow the Player to drop item. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Obsidian|Input")
	float DropRadius = 200.0f;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USplineComponent> AutoRunSplineComp;

private:
	void CursorTrace();
	
	/**
	 * Mouse Movement.
	 */
	
	bool CanMoveMouse() const;
	bool CanContinuouslyMoveMouse() const;
	void AutoRun();
	void AutoRunToClickedLocation();

	/**
	 * Using Item.
	 */
	
	void DragUsableItemIcon() const;
	
	/**
	 * Item Pick up.
	 */
	
	UFUNCTION(Client, Reliable)
	void ClientStartApproachingOutOfRangeItem(const FVector_NetQuantize10& ToDestination, AObsidianDroppableItem* ItemToPickUp, const EObsidianItemPickUpType PickUpType);
	
	UFUNCTION(Server, Reliable)
	void ServerUseItem(UObsidianInventoryItemInstance* UsingInstance, const FIntPoint& OnSlotPosition);

	void AutomaticallyPickupOutOfRangeItem();
	void DragOutOfRangeItem();

	/** If item is out of picking up range, it will handle getting to the item and picking it up. Will return true if item picking up is handled. */
	bool HandlePickUpIfItemOutOfRange(AObsidianDroppableItem* ItemToPickUp, const EObsidianItemPickUpType PickUpType);

	void DragItem() const;
	
	void StartDraggingItem(const AController* Controller);
	void StopDraggingItem(const AController* Controller);

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
	
	bool CanDropItem() const;
	
	/**
	 * Interaction.
	 */

	UFUNCTION(Client, Reliable)
	void ClientStartApproachingOutOfRangeInteractionTarget(const FVector_NetQuantize10& ToDestination);
	
	UFUNCTION(Server, Reliable)
	void ServerStartInteraction(const TScriptInterface<IObsidianInteractionInterface>& InteractionTarget);
	void InteractWithOutOfRangeTarget();

	UFUNCTION(Client, Reliable)
	void ClientTriggerInteraction(const TScriptInterface<IObsidianInteractionInterface>& InteractionTarget);
	
	bool IsHoveringOverInteractionTarget() const;

	/** If Interaction target is out of interaction range, it will handle getting to the interaction target and interacting with it. Will return true if interaction was handled here. */
	bool HandleOutOfRangeInteraction(const TScriptInterface<IObsidianInteractionInterface>& InteractionTarget, const FVector& TargetLocation);
	
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obsidian|Items", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AObsidianDroppableItem> DroppableItemClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obsidian|Items", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UObsidianDraggedItem> DraggedItemWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obsidian|Items", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UObsidianDraggedItem_Simple> DraggedUsableItemWidgetClass;
	
	/** Used for both highlighting and movement to avoid getting it twice, we get this in CursorTrace */
	FHitResult CursorHit;

	/**
	 * Mouse auto run.
	 */
	FVector CachedDestination = FVector::ZeroVector;
	float FollowTime = 0.f;
	bool bAutoRunning = false;
	bool bWasAutoMovingLastTick = false;

	/**
	 * Interaction.
	 */
	bool bWantsToInteract = false;
	bool bAutoRunToInteract = false;
	bool bActivelyInteracting = false;
	TScriptInterface<IObsidianInteractionInterface> CachedInteractionTarget;
	FOnArrivedAtAcceptableInteractionRangeSignature OnArrivedAtAcceptableInteractionRange;
	
	UPROPERTY(EditDefaultsOnly, meta=(AllowPrivateAccess = true), Category = "Obsidian|Debug")
	bool bDebugInteraction = false;
	
	/**
	 * Pickup.
	 */
	bool bAutoRunToPickupItemByLabel = false;
	TObjectPtr<AObsidianDroppableItem> CachedDroppableItemToPickup;
	FOnArrivedAtAcceptableItemPickupRangeSignature OnArrivedAtAcceptableItemPickupRange;

	/**
	 * Highlight.
	 */
	IObsidianHighlightInterface* LastHighlightedActor = nullptr;
	IObsidianHighlightInterface* CurrentHighlightedActor = nullptr;

	/**
	 * Dragged items.
	 */
	UPROPERTY()
	TObjectPtr<UObsidianDraggedItem> DraggedItemWidget;

	UPROPERTY()
	TObjectPtr<UObsidianDraggedItem_Simple> DraggedUsableItemWidget;

	UPROPERTY(ReplicatedUsing = OnRep_DraggedItem)
	FDraggedItem DraggedItem = FDraggedItem();
	
#if 0 // https://github.com/intrxx/Obsidian/commit/e3eda3899a1b39ec1952221a24bce0b40b7be769
	FVector CachedItemDropLocation = FVector::ZeroVector;
#endif
	
	UPROPERTY()
	TObjectPtr<UObsidianInventoryItemInstance> UsingItemInstance = nullptr;
	UPROPERTY()
	TObjectPtr<UObsidianItem> CachedUsingInventoryItemWidget = nullptr;

	bool bUsingItem = false;
	bool bDraggingItem = false;
	bool bItemAvailableForDrop = false;
	bool bJustDroppedItem = false;
};
