// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"

#include "ObsidianItemStoragePanelBase.h"
#include "ObsidianTypes/ItemTypes/ObsidianItemTypes.h"
#include "Slots/ObsidianSlot_GridSlot.h"
#include "Slots/ObsidianSlotBase.h"

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
	bool ConstructStashPanel(const int32 InGridWidthOverride, const int32 InGridHeightOverride, const FGameplayTag& InStashTag);
	
	UObsidianSlot_GridSlot* GetSlotByPosition(const FIntPoint& InBySlotPosition);
	const FObsidianGridSlotData* GetSlotDataAtGridPosition(const FIntPoint& InAtGridPosition) const;
	UObsidianItem* GetItemWidgetAtGridPosition(const FIntPoint& InAtGridPosition) const;
	bool IsGridSlotOccupied(const FIntPoint& InAtGridPosition) const;
	
	void AddItemWidget(UObsidianItem* InItemWidget, const FObsidianItemWidgetData& InItemWidgetData);
	void HandleItemRemoved(const FObsidianItemWidgetData& InItemWidgetData);
	void HandleItemChanged(const FObsidianItemWidgetData& InItemWidgetData);

	void HandleHighlightingItems(const TArray<FObsidianItemPosition>& InItemsToHighlight);
	void ClearUsableItemHighlight();
	
protected:
	virtual void HandleWidgetControllerSet() override;
	virtual void NativeDestruct() override;
	
	bool ConstructGrid(const int32 InGridWidth, const int32 InGridHeight);
	
	void RegisterGridItemWidget(const FObsidianItemPosition& InItemPosition, UObsidianItem* InItemWidget,
		const FIntPoint InGridSpan);

	void OnGridSlotHover(UObsidianSlot_GridSlot* InAffectedSlot, const bool bInEntered);
	void OnGridSlotLeftMouseButtonDown(const UObsidianSlot_GridSlot* InAffectedSlot,
		const FObsidianItemInteractionFlags& InInteractionFlags);
	void OnGridSlotRightMouseButtonDown(const UObsidianSlot_GridSlot* InAffectedSlot,
		const FObsidianItemInteractionFlags& InInteractionFlags);
	
	void ConstructItemPosition(FObsidianItemPosition& OutItemPosition, const FIntPoint InSlotPositionOverride) const;
	
protected:
	UPROPERTY(EditAnywhere, Category = "Obsidian|Setup")
	TSubclassOf<UObsidianSlot_GridSlot> GridSlotClass;

	UPROPERTY(EditAnywhere, Category = "Obsidian|Setup")
	float SlotTileSize = 68.0f;

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCanvasPanel> Root_CanvasPanel;

private:
	void OffsetGridPositionByItemSpan(FIntPoint InDraggedItemGridSpan, FIntPoint& InOutOriginalPosition) const;
	void SetSlotStateForGridSlots(const FIntPoint InOriginPosition, const FIntPoint InItemGridSpan,
		const EObsidianItemSlotState InSlotState);
	void ResetGridSlotsState();
	
private:
	UPROPERTY()
	TObjectPtr<UObInventoryItemsWidgetController> InventoryItemsWidgetController;
	
	UPROPERTY()
	TMap<FIntPoint, FObsidianGridSlotData> GridSlotDataMap;
	
	/** Array of slots that are affected by item hover, to clear it later. */
	UPROPERTY()
	TArray<UObsidianSlot_GridSlot*> AffectedGridSlots;

	UPROPERTY()
	TArray<TObjectPtr<UObsidianItem>> HighlightedItems;
};
