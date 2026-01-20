// Copyright 2026 out of sCope team - intrxx

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project
#include "Interaction/ObsidianHighlightInterface.h"
#include "ObsidianTypes/ItemTypes/ObsidianItemTypes.h"
#include "Interaction/ObsidianInteractionInterface.h"

#include "Gameplay/ObsidianWorldCollectable.h"
#include "ObsidianDroppableItem.generated.h"

class UObInventoryItemsWidgetController;
class USplineComponent;
class UTimelineComponent;
class AObsidianPlayerController;
class UObsidianItemDescriptionBase;
class UObsidianItemWorldName;
class UObsidianItemDragDropOperation;
class UObsidianDraggedItem;
class  UWidgetComponent;

/**
 * Base class for all droppable items in Obsidian.
 */
UCLASS()
class OBSIDIAN_API AObsidianDroppableItem : public AObsidianWorldCollectable, public IObsidianHighlightInterface, public IObsidianInteractionInterface
{
	GENERATED_BODY()

public:
	AObsidianDroppableItem(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	void InitializeItem(const FDraggedItem& DraggedItem);
	void InitializeItem(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef, const FObsidianItemGeneratedData& InGeneratedData);

	//TODO(intrxx) this shouldn't be public?
	virtual void AddItemInstance(UObsidianInventoryItemInstance* InstanceToAdd) override;
	virtual void AddItemDefinition(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef, const FObsidianItemGeneratedData& InGeneratedData) override;

	//~ Start of HighlightInterface
	virtual AActor* GetHighlightAvatarActor() override;
	virtual void StartHighlight() override;
	virtual void StopHighlight() override;
	//~ End of HighlightInterface

	//~ Start of InteractionInterface
	virtual AActor* GetInteractionActor() override;
	virtual bool CanInteract() override;
	virtual float GetInteractionRadius() override;
	virtual void Interact(AObsidianPlayerController* InteractingPlayerController) override;
	//~ End of InteractionInterface

	/**
	 * Use only on Server.
	 * Updates the stack count of dropped item after trying to pick it up.
	 * Must be called after potential stack changes of the item, handles destroying the item if stack count is 0 or negative,
	 * although it should never be negative, so It will warn you.
	 */
	void UpdateDroppedItemStacks(const int32 NewDroppedItemStacks);

	void DestroyDroppedItem();

protected:
	virtual void BeginPlay() override;
	virtual void Destroyed() override;
	virtual void OnRep_PickupContent() override;
	
	void OnItemMouseHover(const bool bMouseEnter);
	void OnItemMouseButtonDown(const int32 PlayerIndex, const FObsidianItemInteractionFlags& InteractionFlags);
	
	bool InitializeWorldName();

private:
	/** Pickups available Item Instance, returns true if item with whole stacks was picked up. */
	bool PickupItemInstance(const bool bLeftControlDown, const AObsidianPlayerController* PickingPlayerController);
	
	/** Pickups available Item Def, returns true if item with whole stacks was picked up. */
	bool PickupItemDef(const bool bLeftControlDown, const AObsidianPlayerController* PickingPlayerController);
	
	/** Sets up any Appearance related thing, needs to be called after setting the item instance itself. */
	void SetupItemAppearanceFromInstance() const;
	
	/** Sets up any Appearance related thing, needs to be called after setting the item def itself. */
	void SetupItemAppearanceFromDefinition() const;

	bool InitItemWorldName() const;
	void InitDropRouteAnimation();

	UFUNCTION()
	void UpdateItemDropAnimation(float UpdateAlpha);

	void CreateItemDescription();
	void DestroyItemDescription();
	void UpdateStacksOnActiveItemDescription(const UObsidianInventoryItemInstance* ItemInstance);
	void UpdateStacksOnActiveItemDescription(const int32 StacksToSet);

	UFUNCTION()
	void OnRep_DroppedItemStacks();
	
private:
	UPROPERTY(VisibleAnywhere, Category = "Obsidian", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> RootSceneComponent;
	
	UPROPERTY(VisibleAnywhere, Category = "Obsidian", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> StaticMeshComp;
	
	UPROPERTY(VisibleAnywhere, Category = "Obsidian", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UWidgetComponent> WorldItemNameWidgetComp;
	
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|DropAnimation", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCurveFloat> DropLocationCurve;
	
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|DropAnimation", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USplineComponent> ItemDropSplineComp;
	
	UPROPERTY()
	TObjectPtr<UObsidianItemWorldName> ItemWorldName;

	UPROPERTY()
	UObInventoryItemsWidgetController* CachedInventoryWidgetController;
	
	UPROPERTY(ReplicatedUsing = OnRep_DroppedItemStacks)
	int32 DroppedItemStacks = 1;

	bool bInitializedItemName = false;

	/**
	 * Item Drop Route animation.
	 */

	/** Initial item rotation, randomised. */
	FRotator InitialItemRotation = FRotator::ZeroRotator;

	/** Final item rotation, the rotation the item is spawned with, most likely aligned to the ground. */
	FRotator FinalItemRotation = FRotator::ZeroRotator;
};
