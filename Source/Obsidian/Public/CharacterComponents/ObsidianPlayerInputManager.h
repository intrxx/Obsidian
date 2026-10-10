// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "Components/PawnComponent.h"
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

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

namespace ObsidianPlayerInputStatics
{
	inline constexpr float InteractionRadius = 200.0f;
	inline constexpr float InteractionRadiusSquared = FMath::Square(InteractionRadius);
	inline constexpr float AutoRunAcceptanceRadius = 30.0f;
	inline constexpr float InteractionRangeTolerance = AutoRunAcceptanceRadius + 10.0f;
}

/**
 * Component that manages every type of Player Input in obsidian. This includes regular native input, interactions,
 * highlights and Inventory Items handling.
 */
UCLASS()
class OBSIDIAN_API UObsidianPlayerInputManager : public UPawnComponent
{
	GENERATED_BODY()
	
public:
	UObsidianPlayerInputManager(const FObjectInitializer& InObjectInitializer = FObjectInitializer::Get());
	
	virtual void TickComponent(float InDeltaTime, ELevelTick InTickType,
		FActorComponentTickFunction* InThisTickFunction) override;

	void InitializePlayerInput(UInputComponent* InInputComponent);
	
	/** Returns the player input manager component if one exists on the specified actor. */
	UFUNCTION(BlueprintPure, Category = "Obsidian|PlayerInputManager")
	static UObsidianPlayerInputManager* FindPlayerInputManager(const AActor* InActor)
	{
		return (InActor ? InActor->FindComponentByClass<UObsidianPlayerInputManager>() : nullptr);
	}
	
	AObsidianHUD* GetObsidianHUD() const;

	void TriggerInteraction(AActor* InInteractionActor);
	
protected:
	void Input_AbilityInputTagPressed(FGameplayTag InInputTag);
	void Input_AbilityInputTagReleased(FGameplayTag InInputTag);
	
	void Input_MoveKeyboard(const FInputActionValue& InInputActionValue);
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
	void Input_ToggleWalk();

	void Input_OpenGameplayMenu();

	void Input_ToggleHighlight();
	
protected:
	/** Time Threshold to know if it was a short press */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Obsidian|Input")
	float ShortPressThreshold = 0.3f;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USplineComponent> AutoRunSplineComp;

private:
	void CursorTrace();
	
	/**
	 * Movement.
	 */
	
	bool CanMoveMouse() const;
	bool CanContinuouslyMoveMouse() const;
	void AutoRun();
	void AutoRunToClickedLocation();

	UFUNCTION(Server, Reliable)
	void ServerToggleWalk();
	
	/**
	 * Interaction.
	 */

	UFUNCTION(Client, Reliable)
	void ClientStartApproachingOutOfRangeInteractionTarget(const FVector_NetQuantize10& InToDestination);
	
	UFUNCTION(Server, Reliable)
	void ServerStartInteraction(const TScriptInterface<IObsidianInteractionInterface>& InInteractionTarget);
	void InteractWithOutOfRangeTarget();

	UFUNCTION(Client, Reliable)
	void ClientTriggerInteraction(const TScriptInterface<IObsidianInteractionInterface>& InInteractionTarget);
	UFUNCTION(Client, Reliable)
	void ClientAbandonInteraction();
	
	bool IsHoveringOverInteractionTarget() const;
	void StopOngoingInteraction();

	/** If Interaction target is out of interaction range, it will handle getting to the interaction target and interacting with it. Will return true if interaction was handled here. */
	bool HandleOutOfRangeInteraction(const TScriptInterface<IObsidianInteractionInterface>& InInteractionTarget, const FVector& InTargetLocation);
	
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
