// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project


#include "UI/ObsidianWidgetBase.h"
#include "ObsidianInventoryGrid.generated.h"

class UCanvasPanel;
class UObsidianInventory;
class UObsidianItem;
class UObsidianItemSlot_Inventory;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnInventoryGridSlotPressedSignature, const FIntPoint& SlotPosition, const bool bWithShiftDown);

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianInventoryGrid : public UObsidianWidgetBase
{
	GENERATED_BODY()

public:
	void ConstructInventoryGrid(UObsidianInventory* InOwningInventory, const int32 InventoryGridWidth, const int32 InventoryGridHeight);

	UObsidianItemSlot_Inventory* GetSlotByPosition(const FIntPoint& BySlotPosition);

	void OnInventorySlotHover(const UObsidianItemSlot_Inventory* AffectedSlot, const bool bEntered);
	void OnInventorySlotMouseButtonDown(const UObsidianItemSlot_Inventory* AffectedSlot, const bool bShiftDown) const;

	void AddItemToGrid(UObsidianItem* ItemWidget, const float ItemSlotPadding = 0.0f);

public:
	FOnInventoryGridSlotPressedSignature OnInventoryGridSlotPressedDelegate;
	
protected:
	UPROPERTY(EditAnywhere, Category = "Obsidian|Setup")
	TSubclassOf<UObsidianItemSlot_Inventory> InventorySlotClass;

	UPROPERTY(EditAnywhere, Category = "Obsidian|Setup")
	float SlotTileSize = 68.0f;
	
	UPROPERTY()
	TArray<TObjectPtr<UObsidianItemSlot_Inventory>> GridSlots;

	/** Array of slots that are affected by item hover, to clear it later. */
	UPROPERTY()
	TArray<UObsidianItemSlot_Inventory*> AffectedInventorySlots;

	UPROPERTY()
	TWeakObjectPtr<UObsidianInventory> OwningInventory;

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCanvasPanel> Root_CanvasPanel;
};
