// Copyright 2026 out of sCope team - intrxx

#pragma once

#include <CoreMinimal.h>
#include <GameplayTagContainer.h>
#include <Components/PawnComponent.h>

#include "ObsidianTypes/ItemTypes/ObsidianItemTypes.h"

#include "ObsidianPlayerInputManager.generated.h"

class UObsidianItemManagerComponent;
struct FInputActionValue;

class UObsidianCraftingComponent;
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
class UCommonActivatableWidget;

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
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;

	void InitializePlayerInput(UInputComponent* InputComponent);
	
	/** Returns the player input manager component if one exists on the specified actor. */
	UFUNCTION(BlueprintPure, Category = "Obsidian|PlayerInputManager")
	static UObsidianPlayerInputManager* FindPlayerInputManager(const AActor* Actor)
	{
		return (Actor ? Actor->FindComponentByClass<UObsidianPlayerInputManager>() : nullptr);
	}
	
	AObsidianHUD* GetObsidianHUD() const;
	
protected:
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

	void Input_OpenGameplayMenu();

	void Input_ToggleHighlight();
	
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
	 * Item Pick up.
	 */
	
	UFUNCTION(Client, Reliable)
	void ClientStartApproachingOutOfRangeItem(const FVector_NetQuantize10& ToDestination, AObsidianDroppableItem* ItemToPickUp,
		const EObsidianItemPickUpType PickUpType);
	
	void AutomaticallyPickupOutOfRangeItem();
	void DragOutOfRangeItem();

	/**
	 * If item is out of picking up range, it will handle getting to the item and picking it up. Will return true
	 * if item picking up is handled.
	 */
	bool HandlePickUpIfItemOutOfRange(AObsidianDroppableItem* ItemToPickUp, const EObsidianItemPickUpType PickUpType);
	
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
	void StopOngoingInteraction();

	/** If Interaction target is out of interaction range, it will handle getting to the interaction target and interacting with it. Will return true if interaction was handled here. */
	bool HandleOutOfRangeInteraction(const TScriptInterface<IObsidianInteractionInterface>& InteractionTarget, const FVector& TargetLocation);
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|UI")
	TSoftClassPtr<UCommonActivatableWidget> GameplayMenuClass;
	
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
	TScriptInterface<IObsidianInteractionInterface> ActiveInteractionTarget;
	FOnArrivedAtAcceptableInteractionRangeSignature OnArrivedAtAcceptableInteractionRange;
	
	UPROPERTY(EditDefaultsOnly, meta=(AllowPrivateAccess = true), Category = "Obsidian|Debug")
	bool bDebugInteraction = false;
	
	/**
	 * Pickup.
	 */
	bool bAutoRunToPickupItemByLabel = false;
	UPROPERTY()
	TObjectPtr<AObsidianDroppableItem> CachedDroppableItemToPickup;
	FOnArrivedAtAcceptableItemPickupRangeSignature OnArrivedAtAcceptableItemPickupRange;

	/**
	 * Highlight.
	 */
	IObsidianHighlightInterface* LastHighlightedActor = nullptr;
	IObsidianHighlightInterface* CurrentHighlightedActor = nullptr;

	/**
	 * Item Crafting
	 */

	UPROPERTY()
	TObjectPtr<UObsidianCraftingComponent> OwnerCraftingComponent = nullptr;

	/**
	 * Item Manager
	 */

	UPROPERTY()
	TObjectPtr<UObsidianItemManagerComponent> OwnerItemManagerComponent = nullptr;
};
