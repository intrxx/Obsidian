// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project
#include "Interaction/ObsidianHighlightInterface.h"
#include "ObsidianTypes/ObsidianItemTypes.h"

#include "Gameplay/ObsidianWorldCollectable.h"
#include "ObsidianDroppableItem.generated.h"

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
class OBSIDIAN_API AObsidianDroppableItem : public AObsidianWorldCollectable, public IObsidianHighlightInterface
{
	GENERATED_BODY()

public:
	AObsidianDroppableItem(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	void InitializeItem(const FDraggedItem& DraggedItem);

	virtual void AddItemInstance(UObsidianInventoryItemInstance* InstanceToAdd) override;
	virtual void AddItemDefinition(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef, const int32 ItemStacks) override;

	//~ Start of HighlightInterface
	virtual AActor* GetHighlightAvatarActor() override;
	virtual void StartHighlight() override;
	virtual void StopHighlight() override;
	//~ End of HighlightInterface

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
	
	virtual void OnRep_PickupContent() override;
	
	UFUNCTION()
	void HandleActorClicked(AActor* AffectedActor, FKey ButtonPressed);
	
	void OnItemMouseHover(const bool bMouseEnter);
	void OnItemMouseButtonDown(const int32 PlayerIndex, const bool bLeftControlDown);
	
	bool InitializeWorldName();

private:
	/** Pickups available Item Instance, returns true if item with whole stacks was picked up. */
	bool PickupItemInstance(const bool bLeftControlDown, AObsidianPlayerController* PickingPlayerController);
	
	/** Pickups available Item Def, returns true if item with whole stacks was picked up. */
	bool PickupItemDef(const bool bLeftControlDown, AObsidianPlayerController* PickingPlayerController);
	
	/** Sets up any Appearance related thing, needs to be called after setting the item instance itself. */
	void SetupItemAppearanceFromInstance() const;
	
	/** Sets up any Appearance related thing, needs to be called after setting the item def itself. */
	void SetupItemAppearanceFromDefinition() const;

	bool InitItemWorldName() const;
	void InitDropRouteAnimation();

	UFUNCTION()
	void UpdateItemDropAnimation(float UpdateAlpha);

	UObsidianItemDescriptionBase* CreateItemDescription();
	void DestroyItemDescription();
	void UpdateStacksOnActiveItemDescription(const UObsidianInventoryItemInstance* ItemInstance) const;
	void UpdateStacksOnActiveItemDescription(const int32 StacksToSet) const;

	UFUNCTION()
	void OnRep_DroppedItemStacks();
	
private:
	UPROPERTY(VisibleAnywhere, Category = "Obsidian", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> StaticMeshComp;
	
	UPROPERTY(VisibleAnywhere, Category = "Obsidian", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UWidgetComponent> WorldItemNameWidgetComp;
	
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|DropAnimation", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCurveFloat> DropLocationCurve;
	
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|DropAnimation", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USplineComponent> ItemDropSplineComp;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UObsidianItemWorldName> ItemWorldNameClass;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UObsidianDraggedItem> DraggedItemWidgetClass;

	UPROPERTY()
	TObjectPtr<UObsidianItemWorldName> ItemWorldName;

	UPROPERTY()
	TObjectPtr<UObsidianItemDescriptionBase> ActiveItemDescription;

	UPROPERTY(ReplicatedUsing = OnRep_DroppedItemStacks)
	int32 DroppedItemStacks = 1;

	bool bInitializedItemName = false;

	/**
	 * Item Drop Route animation.
	 */
	FRotator InitialRotation = FRotator::ZeroRotator;
	float RandomYawRotation = 0.0f;
};
