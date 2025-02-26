// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/PawnComponent.h"
#include "ObsidianTypes/ObsidianItemTypes.h"
#include "ObsidianHeroComponent.generated.h"

class AObsidianPlayerController;
class UObsidianInventoryItemInstance;
struct FInputActionValue;
class AObsidianDroppableItem;
class USplineComponent;
class AObsidianHUD;
class IObsidianHighlightInterface;
class UObsidianDraggedItem;
class UObsidianInventoryItemDefinition;

/**
 * Component that manages hero related things like input
 */
UCLASS()
class OBSIDIAN_API UObsidianHeroComponent : public UPawnComponent
{
	GENERATED_BODY()
public:
	UObsidianHeroComponent(const FObjectInitializer& ObjectInitializer);
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

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
	
	bool IsDraggingAnItem() const
	{
		return bDraggingItem;
	}

	void SetCursorOverUI(const bool bInOverUI)
	{
		bCursorOverUI = bInOverUI;
	}
	
	void InitializePlayerInput(UInputComponent* InputComponent);
	
	bool DropItem();
	
	UFUNCTION(Server, Reliable)
	void ServerAddItemToInventoryAtSlot(const FVector2D& SlotPosition, const bool bShiftDown);
	
	UFUNCTION(Server, Reliable)
	void ServerAddStacksFromDraggedItemToItemAtSlot(const FVector2D& SlotPosition, const int32 StacksToAddOverride = -1);

	UFUNCTION(Server, Reliable)
	void ServerTakeoutFromItem(UObsidianInventoryItemInstance* ItemInstance, const int32 StacksToTake);
	
	UFUNCTION(Server, Reliable)
	void ServerGrabDroppableItemToCursor(AObsidianDroppableItem* ItemToPickup);
	
	UFUNCTION(Server, Reliable)
	void ServerGrabInventoryItemToCursor(UObsidianInventoryItemInstance* InstanceToGrab);
	
	UFUNCTION(Server, Reliable)
	void ServerPickupItemDef(AObsidianDroppableItem* ItemToPickup);

	UFUNCTION(Server, Reliable)
	void ServerPickupItemInstance(AObsidianDroppableItem* ItemToPickup);

	//~ Start of UObject interface
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	virtual void ReadyForReplication() override;
	//~ End of UObject interface
	
protected:
	UFUNCTION()
	void OnRep_DraggedItem();
	
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
	
	void StartDraggingItem();
	void StopDraggingItem();
	
	UFUNCTION(Server, Reliable)
	void ServerHandleDroppingItem(const FVector& HitLocation);
	bool CanDropItem() const;
	
	void UpdateStacksOnDraggedItemWidget(const int32 InStacks);

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obsidian|Items", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AObsidianDroppableItem> DroppableItemClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Obsidian|Items", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UObsidianDraggedItem> DraggedItemWidgetClass;
	
	/** Used for both highlighting and movement to avoid getting it twice, we get this in CursorTrace */
	FHitResult CursorHit;
	
	FVector CachedDestination = FVector::ZeroVector;
	float FollowTime = 0.f;
	bool bAutoRunning = false;
	
	IObsidianHighlightInterface* LastHighlightedActor = nullptr;
	IObsidianHighlightInterface* CurrentHighlightedActor = nullptr;

	UPROPERTY()
	TObjectPtr<UObsidianDraggedItem> DraggedItemWidget;

	UPROPERTY(ReplicatedUsing = OnRep_DraggedItem)
	FDraggedItem DraggedItem = FDraggedItem();
	
	bool bDraggingItem = false;
	bool bItemAvailableForDrop = false;
	bool bJustDroppedItem = false;
	
	bool bCursorOverUI = false;
};

