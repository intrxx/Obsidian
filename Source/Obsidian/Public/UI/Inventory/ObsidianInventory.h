// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "UI/ObsidianMainOverlayWidgetBase.h"
#include "ObsidianInventory.generated.h"

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

public:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UGridPanel> Slots_GridPanel;

private:
	/** Function that triggers when automatically adding item. E.g. from the ground when inventory is hidden. */
	void OnItemAdded(UTexture2D* ItemImage, const FVector2D DesiredPosition, const FVector2D GridSpan);

	void SetupGrid();

	void OnItemLeftMouseButtonPressed(const FVector2D ItemDesiredPosition);
	
	void OnInventorySlotHoverOver(bool bEntered);
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
};
