// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

// ~ Project
#include "ObsidianTypes/ObsidianItemTypes.h"

#include "Components/PawnComponent.h"
#include "ObsidianHeroComponent.generated.h"

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

DECLARE_MULTICAST_DELEGATE(FOnStopUsingItemSignature)

/**
 * Component that manages hero related things like input
 */
UCLASS()
class OBSIDIAN_API UObsidianHeroComponent : public UPawnComponent
{
	GENERATED_BODY()
	
public:
	UObsidianHeroComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void InitializePlayerInput(UInputComponent* InputComponent);
	
	/** Returns the hero component if one exists on the specified actor. */
	UFUNCTION(BlueprintPure, Category = "Obsidian|HeroComp")
	static UObsidianHeroComponent* FindHeroComponent(const AActor* Actor)
	{
		return (Actor ? Actor->FindComponentByClass<UObsidianHeroComponent>() : nullptr);
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
	void ServerPickupItemDef(AObsidianDroppableItem* ItemToPickup);

	UFUNCTION(Server, Reliable)
	void ServerPickupItemInstance(AObsidianDroppableItem* ItemToPickup);

	void UseItem(const FIntPoint& OnSlotPosition, const bool bLeftShiftDown);
	
	UFUNCTION(Server, Reliable)
	void ServerUseItem(UObsidianInventoryItemInstance* UsingInstance, const FIntPoint& OnSlotPosition);
	
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

	void Input_WeaponSwap();
	
protected:
	/** Time Threshold to know if it was a short press */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	float ShortPressThreshold = 0.3f;

	/** Acceptable radius to the destination of autorun */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	float AutoRunAcceptanceRadius = 30.f;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USplineComponent> AutoRunSplineComp;

private:
	bool CanMoveMouse() const;
	
	void AutoRun();
	void CursorTrace();
	void DragItem() const;
	void DragUsableItemIcon() const;
	
	/**
	 * This is a very specific function that is used to determine if the dragged item was changed in a result of replacing it with
	 * another item in the inventory in OnRep_DraggedItem. If it sounds useful be careful when using it.
	 */
	bool DraggedItemWasReplaced(const FDraggedItem& OldDraggedItem) const;
	
	void StartDraggingItem(const AController* Controller);
	void StopDraggingItem(const AController* Controller);
	
	UFUNCTION(Server, Reliable)
	void ServerHandleDroppingItem(const FVector& HitLocation);
	bool CanDropItem() const;
	
	void UpdateStacksOnDraggedItemWidget(const int32 InStacks);

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obsidian|Items", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AObsidianDroppableItem> DroppableItemClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obsidian|Items", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UObsidianDraggedItem> DraggedItemWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obsidian|Items", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UObsidianDraggedItem_Simple> DraggedUsableItemWidgetClass;
	
	/** Used for both highlighting and movement to avoid getting it twice, we get this in CursorTrace */
	FHitResult CursorHit;
	
	FVector CachedDestination = FVector::ZeroVector;
	float FollowTime = 0.f;
	bool bAutoRunning = false;
	
	IObsidianHighlightInterface* LastHighlightedActor = nullptr;
	IObsidianHighlightInterface* CurrentHighlightedActor = nullptr;

	UPROPERTY()
	TObjectPtr<UObsidianDraggedItem> DraggedItemWidget;

	UPROPERTY()
	TObjectPtr<UObsidianDraggedItem_Simple> DraggedUsableItemWidget;

	UPROPERTY(ReplicatedUsing = OnRep_DraggedItem)
	FDraggedItem DraggedItem = FDraggedItem();

	UPROPERTY()
	TObjectPtr<UObsidianInventoryItemInstance> UsingItemInstance = nullptr;
	UPROPERTY()
	TObjectPtr<UObsidianItem> CachedUsingInventoryItemWidget = nullptr;

	bool bUsingItem = false;
	bool bDraggingItem = false;
	bool bItemAvailableForDrop = false;
	bool bJustDroppedItem = false;
};
