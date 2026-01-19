// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include <CoreMinimal.h>

#include "ObsidianTypes/ItemTypes/ObsidianItemTypes.h"
#include "Slots/ObsidianSlotBase.h"

#include "ObsidianItemStoragePanelBase.h"
#include "Slots/ObsidianSlot_GridSlot.h"
#include "ObsidianGridPanel.generated.h"

struct FObsidianItemWidgetData;

class UObInventoryItemsWidgetController;
class UCanvasPanel;
class UObsidianItem;
class UObsidianSlot_GridSlot;

USTRUCT()
struct FObsidianGridSlotData
{
	GENERATED_BODY()

public:
	FObsidianGridSlotData(){};

	bool IsOccupied() const;
	
	void AddNewItem(const FObsidianItemPosition& InPosition, UObsidianItem* InItemWidget,
		const FIntPoint InItemGridSpan);
	void Reset();
	
public:
	UPROPERTY()
	FObsidianItemPosition OriginPosition = FObsidianItemPosition();

	UPROPERTY()
	UObsidianSlot_GridSlot* OwningGridSlot = nullptr;
	
	UPROPERTY()
	UObsidianItem* ItemWidget = nullptr;
	
	UPROPERTY()
	FIntPoint ItemGridSpan = FIntPoint::NoneValue;

protected:
	UPROPERTY()
	bool bOccupied = false;
};


/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianGridPanel : public UObsidianItemStoragePanelBase
{
	GENERATED_BODY()

public:
	bool ConstructInventoryPanel();
	bool ConstructStashPanel(const int32 GridWidthOverride, const int32 GridHeightOverride, const FGameplayTag& InStashTag);
	
	UObsidianSlot_GridSlot* GetSlotByPosition(const FIntPoint& BySlotPosition);
	const FObsidianGridSlotData* GetSlotDataAtGridPosition(const FIntPoint& AtGridPosition) const;
	UObsidianItem* GetItemWidgetAtGridPosition(const FIntPoint& AtGridPosition) const;
	bool IsGridSlotOccupied(const FIntPoint& AtGridPosition) const;
	
	void AddItemWidget(UObsidianItem* ItemWidget, const FObsidianItemWidgetData& ItemWidgetData);
	void HandleItemRemoved(const FObsidianItemWidgetData& ItemWidgetData);
	void HandleItemChanged(const FObsidianItemWidgetData& ItemWidgetData);

protected:
	virtual void HandleWidgetControllerSet() override;
	virtual void NativeDestruct() override;
	
	bool ConstructGrid(const int32 GridWidth, const int32 GridHeight);
	
	void RegisterGridItemWidget(const FObsidianItemPosition& ItemPosition, UObsidianItem* ItemWidget,
		const FIntPoint GridSpan);

	void OnGridSlotHover(UObsidianSlot_GridSlot* AffectedSlot, const bool bEntered);
	void OnGridSlotLeftMouseButtonDown(const UObsidianSlot_GridSlot* AffectedSlot,
		const FObsidianItemInteractionFlags& InteractionFlags);
	void OnGridSlotRightMouseButtonDown(const UObsidianSlot_GridSlot* AffectedSlot,
		const FObsidianItemInteractionFlags& InteractionFlags);
	
	void ConstructItemPosition(FObsidianItemPosition& ItemPosition, const FIntPoint SlotPositionOverride) const;
	
protected:
	UPROPERTY(EditAnywhere, Category = "Obsidian|Setup")
	TSubclassOf<UObsidianSlot_GridSlot> GridSlotClass;

	UPROPERTY(EditAnywhere, Category = "Obsidian|Setup")
	float SlotTileSize = 68.0f;

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCanvasPanel> Root_CanvasPanel;

private:
	void OffsetGridPositionByItemSpan(FIntPoint DraggedItemGridSpan, FIntPoint& OriginalPosition) const;
	void SetSlotStateForGridSlots(const FIntPoint OriginPosition, const FIntPoint ItemGridSpan,
		const EObsidianItemSlotState SlotState);
	void ResetGridSlotsState();
	
private:
	UPROPERTY()
	TObjectPtr<UObInventoryItemsWidgetController> InventoryItemsWidgetController;
	
	UPROPERTY()
	TMap<FIntPoint, FObsidianGridSlotData> GridSlotDataMap;
	
	/** Array of slots that are affected by item hover, to clear it later. */
	UPROPERTY()
	TArray<UObsidianSlot_GridSlot*> AffectedGridSlots;
};
