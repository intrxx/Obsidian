// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "UI/ObsidianMainOverlayWidgetBase.h"
#include "ObsidianInventory.generated.h"

struct FObsidianItemVisuals;
class UObsidianItem;
class UObsidianInventoryItemDefinition;
class UObsidianInventorySlot;
class UObsidianInventoryItemInstance;
class UGridPanel;
class UOverlay;
class UGridSlot;
class UObsidianInventoryWidgetController;

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianInventory : public UObsidianMainOverlayWidgetBase
{
	GENERATED_BODY()

public:
	UObsidianInventory();
	
	virtual void NativeConstruct() override;
	virtual void HandleWidgetControllerSet() override;

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UGridPanel> Slots_GridPanel;

private:
	/** Function that triggers when automatically adding item. E.g. from the ground when inventory is hidden. */
	void OnItemAdded(const FObsidianItemVisuals& ItemVisuals);

	void SetupGrid();

	void OnItemLeftMouseButtonPressed(const FVector2D& ItemDesiredPosition, UObsidianItem* ItemWidget);
	void OnInventorySlotHover(bool bEntered, UObsidianInventorySlot* AffectedSlot);
	void OnInventorySlotMouseButtonDown(const FVector2D& SlotPosition);
	
private:
	UPROPERTY()
	TObjectPtr<UObsidianInventoryWidgetController> InventoryWidgetController;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Setup")
	TSubclassOf<UObsidianItem> ItemWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Setup")
	TSubclassOf<UObsidianInventorySlot> InventorySlotClass;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Setup")
	int32 InventoryGridWidth = 12;
	
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Setup")
	int32 InventoryGridHeight = 5;
	
	int32 InventoryGridSize;
	TMap<FVector2D, UObsidianInventorySlot*> InventoryLocationToSlotMap;

	/** Array of slots that are affected by item hover, to clear it later. */
	TArray<UObsidianInventorySlot*> AffectedSlots;
};
