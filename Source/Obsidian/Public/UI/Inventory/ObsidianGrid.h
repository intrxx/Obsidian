// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include <CoreMinimal.h>

#include "ObsidianTypes/ItemTypes/ObsidianItemTypes.h"

#include "UI/ObsidianWidgetBase.h"
#include "ObsidianGrid.generated.h"

struct FObsidianItemWidgetData;

class UObInventoryItemsWidgetController;
class UCanvasPanel;
class UObsidianItem;
class UObsidianItemSlot_GridSlot;

USTRUCT()
struct FObsidianOccupiedPlacement
{
	GENERATED_BODY()

	FObsidianOccupiedPlacement(){};

public:
	void Reset();
	
public:
	UPROPERTY()
	UObsidianItem* ItemWidget = nullptr;
	
	UPROPERTY()
	FIntPoint OriginPosition = FIntPoint::NoneValue;

	UPROPERTY()
	FIntPoint ItemGridSpan = FIntPoint::NoneValue;

	UPROPERTY()
	bool bOccupied = false;
};


/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianGrid : public UObsidianWidgetBase
{
	GENERATED_BODY()

public:
	void ConstructGrid(const EObsidianGridOwner InGridOwner, const int32 GridWidth, const int32 GridHeight,
		const FGameplayTag& OptionalStashTag = FGameplayTag::EmptyTag);

	UObsidianItemSlot_GridSlot* GetSlotByPosition(const FIntPoint& BySlotPosition);
	FObsidianOccupiedPlacement* GetSlotPlacementAtGridPosition(const FIntPoint& AtGridPosition);
	UObsidianItem* GetItemWidgetAtGridPosition(const FIntPoint& AtGridPosition);
	bool IsGridSlotOccupied(const FIntPoint& AtGridPosition) const;
	
	void AddItemWidget(UObsidianItem* ItemWidget, const FObsidianItemWidgetData& ItemWidgetData);
	void HandleItemRemoved(const FIntPoint& GridSlot);
	void HandleItemStackChanged(const FIntPoint& AtGridSlot, const uint8 NewStackCount);

protected:
	virtual void NativeDestruct() override;
	
	virtual void HandleWidgetControllerSet() override;
	
	void RegisterInventoryItemWidget(const FIntPoint& GridSlot, UObsidianItem* ItemWidget);

	void OnInventorySlotHover(UObsidianItemSlot_GridSlot* AffectedSlot, const bool bEntered);
	void OnInventorySlotLeftMouseButtonDown(const UObsidianItemSlot_GridSlot* AffectedSlot,
		const FObsidianItemInteractionFlags& InteractionFlags);
	void OnInventorySlotRightMouseButtonDown(const UObsidianItemSlot_GridSlot* AffectedSlot,
		const FObsidianItemInteractionFlags& InteractionFlags);
	
protected:
	UPROPERTY(EditAnywhere, Category = "Obsidian|Setup")
	TSubclassOf<UObsidianItemSlot_GridSlot> GridSlotClass;

	UPROPERTY(EditAnywhere, Category = "Obsidian|Setup")
	float SlotTileSize = 68.0f;
	
	UPROPERTY()
	TMap<FIntPoint, UObsidianItemSlot_GridSlot*> GridSlotsMap;

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCanvasPanel> Root_CanvasPanel;

private:
	void OffsetGridPositionByItemSpan(FIntPoint DraggedItemGridSpan, FIntPoint& OriginalPosition) const;
	
private:
	UPROPERTY()
	TObjectPtr<UObInventoryItemsWidgetController> InventoryItemsWidgetController;
	
	UPROPERTY()
	TMap<FIntPoint, FObsidianOccupiedPlacement> InventorizedItemWidgetMap;
	
	UPROPERTY()
	EObsidianGridOwner GridOwner = EObsidianGridOwner::None;

	UPROPERTY()
	FGameplayTag StashTag = FGameplayTag::EmptyTag;

	/** Array of slots that are affected by item hover, to clear it later. */
	UPROPERTY()
	TArray<UObsidianItemSlot_GridSlot*> AffectedGridSlots;
};
