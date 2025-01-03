// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/ObsidianWorldCollectable.h"
#include "ObsidianDroppableItem.generated.h"

class UObsidianItemDragDropOperation;
class UObsidianDraggedItem;
class UObsidianGroundItemDesc;
class  UWidgetComponent;

/**
 * 
 */
UCLASS()
class OBSIDIAN_API AObsidianDroppableItem : public AObsidianWorldCollectable
{
	GENERATED_BODY()

public:
	AObsidianDroppableItem(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void AddItemInstance(UObsidianInventoryItemInstance* InstanceToAdd) override;
	virtual void AddItemDefinition(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef, const int32 ItemStacks) override;
	
protected:
	virtual void BeginPlay() override;

	void OnItemDescMouseHover(const bool bMouseEnter);
	void OnItemDescMouseButtonDown();

private:
	void PickupItemInstance() const;
	void PickupItemDef() const;

	void InitItemDesc(UObsidianGroundItemDesc* GroundItemDesc);

	/** Sets up any Appearance related thing, needs to be called after setting the item instance itself. */
	void SetupItemAppearanceFromInstance();
	/** Sets up any Appearance related thing, needs to be called after setting the item def itself. */
	void SetupItemAppearanceFromDefinition();
	
private:
	UPROPERTY(VisibleAnywhere, Category = "Obsidian", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> StaticMeshComp;
	
	UPROPERTY(VisibleAnywhere, Category = "Obsidian", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UWidgetComponent> GroundItemDescWidgetComp;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UObsidianGroundItemDesc> GroundItemDescClass;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UObsidianDraggedItem> DraggedItemWidgetClass;
};
