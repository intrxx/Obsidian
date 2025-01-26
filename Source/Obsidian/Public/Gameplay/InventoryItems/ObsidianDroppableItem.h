// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/ObsidianWorldCollectable.h"
#include "Interaction/ObsidianHighlightInterface.h"
#include "ObsidianDroppableItem.generated.h"

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

	virtual void AddItemInstance(UObsidianInventoryItemInstance* InstanceToAdd) override;
	virtual void AddItemDefinition(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef, const int32 ItemStacks) override;

	//~ Start of HighlightInterface
	virtual AActor* GetHighlightAvatarActor() override;
	virtual void StartHighlight() override;
	virtual void StopHighlight() override;
	//~ End of HighlightInterface
	
protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void HandleActorClicked(AActor* AffectedActor, FKey ButtonPressed);
	
	void OnItemMouseHover(const bool bMouseEnter);
	void OnItemMouseButtonDown(const bool bLeftControlDown);

private:
	/** Pickups available Item Instance, returns true if item with whole stacks was picked up. */
	bool PickupItemInstance(const bool bLeftControlDown);
	
	/** Pickups available Item Def, returns true if item with whole stacks was picked up. */
	bool PickupItemDef(const bool bLeftControlDown);
	
	/** Sets up any Appearance related thing, needs to be called after setting the item instance itself. */
	void SetupItemAppearanceFromInstance();
	
	/** Sets up any Appearance related thing, needs to be called after setting the item def itself. */
	void SetupItemAppearanceFromDefinition();

	void InitItemDesc() const;

	UObsidianItemDescriptionBase* CreateItemDescription();
	void DestroyItemDescription();
	void UpdateStacksOnActiveItemDescription(const UObsidianInventoryItemInstance* ItemInstance) const;
	void UpdateStacksOnActiveItemDescription(const int32 StacksToSet) const;
	
private:
	UPROPERTY(VisibleAnywhere, Category = "Obsidian", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> StaticMeshComp;
	
	UPROPERTY(VisibleAnywhere, Category = "Obsidian", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UWidgetComponent> WorldItemNameWidgetComp;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UObsidianItemWorldName> GroundItemDescClass;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UObsidianDraggedItem> DraggedItemWidgetClass;

	UPROPERTY()
	TObjectPtr<UObsidianItemWorldName> GroundItemDesc;

	UPROPERTY()
	TObjectPtr<UObsidianItemDescriptionBase> ActiveItemDescription;
};
