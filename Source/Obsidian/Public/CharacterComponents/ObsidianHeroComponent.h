// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/PawnComponent.h"
#include "ObsidianHeroComponent.generated.h"

struct FInputActionValue;
class AObsidianDroppableItem;
class USplineComponent;
class AObsidianHUD;
class IObsidianHighlightInterface;
class UObsidianDraggedItem;

/**
 * Component that manages hero related things like input
 */
UCLASS()
class OBSIDIAN_API UObsidianHeroComponent : public UPawnComponent
{
	GENERATED_BODY()
public:
	UObsidianHeroComponent(const FObjectInitializer& ObjectInitializer);

	/** Returns the hero component if one exists on the specified actor. */
	UFUNCTION(BlueprintPure, Category = "Obsidian|HeroComp")
	static UObsidianHeroComponent* FindHeroComponent(const AActor* Actor)
	{
		return (Actor ? Actor->FindComponentByClass<UObsidianHeroComponent>() : nullptr);
	}
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	void InitializePlayerInput(UInputComponent* InputComponent);
	
	AObsidianHUD* GetObsidianHUD() const;

	void DragItem(UObsidianDraggedItem* InDraggedItem);
	void StopDragging();
	
	void SetDraggedItemClass(TSubclassOf<AObsidianDroppableItem> InDroppableItemClass)
	{
		DroppableItemClass = InDroppableItemClass;
	};
	
	/** Gets the currently dragged item, will be nullptr when the character does not drag any item. */
	UObsidianDraggedItem* GetCurrentlyDraggedItem()
	{
		return DraggedItem;
	}

	bool IsDraggingAnItem() const
	{
		return bDragItem;
	}

	void SetCursorOverUI(const bool bInOverUI)
	{
		bCursorOverUI = bInOverUI;
	}
	
	bool DropItem();
	
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
	void AutoRun();
	void CursorTrace();
	void DragItem();

	bool CanDropItem() const;
	bool HandleDroppingItem();
	
private:
	TSubclassOf<AObsidianDroppableItem> DroppableItemClass;
	
	/** Used for both highlighting and movement to avoid getting it twice, we get this in CursorTrace */
	FHitResult CursorHit;
	
	FVector CachedDestination = FVector::ZeroVector;
	float FollowTime = 0.f;
	bool bAutoRunning = false;
	
	IObsidianHighlightInterface* LastHighlightedActor = nullptr;
	IObsidianHighlightInterface* CurrentHighlightedActor = nullptr;

	TObjectPtr<UObsidianDraggedItem> DraggedItem;
	bool bDragItem = false;
	bool bItemAvailableForDrop = false;
	
	bool bCursorOverUI = false;
};

