// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project
#include "ObsidianTypes/ObsidianItemTypes.h"

#include "UI/ObsidianWidgetBase.h"
#include "ObsidianGrid.generated.h"

class UObsidianInventoryItemsWidgetController;
class UCanvasPanel;
class UObsidianItem;
class UObsidianItemSlot_GridSlot;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnGridSlotPressedSignature, const FIntPoint& SlotPosition, const bool bWithShiftDown);

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianGrid : public UObsidianWidgetBase
{
	GENERATED_BODY()

public:
	void ConstructGrid(UObsidianInventoryItemsWidgetController* InOwningWidgetController, const EObsidianGridOwner InGridOwner, const int32 GridWidth, const int32 GridHeight, const FGameplayTag& OptionalStashTag = FGameplayTag::EmptyTag);

	UObsidianItemSlot_GridSlot* GetSlotByPosition(const FIntPoint& BySlotPosition);

	void OnInventorySlotHover(UObsidianItemSlot_GridSlot* AffectedSlot, const bool bEntered);
	void OnInventorySlotMouseButtonDown(const UObsidianItemSlot_GridSlot* AffectedSlot, const bool bShiftDown) const;

	void AddItemToGrid(UObsidianItem* ItemWidget, const float ItemSlotPadding = 0.0f);

public:
	FOnGridSlotPressedSignature OnGridSlotPressedDelegate;
	
protected:
	UPROPERTY(EditAnywhere, Category = "Obsidian|Setup")
	TSubclassOf<UObsidianItemSlot_GridSlot> GridSlotClass;

	UPROPERTY(EditAnywhere, Category = "Obsidian|Setup")
	float SlotTileSize = 68.0f;
	
	UPROPERTY()
	TArray<UObsidianItemSlot_GridSlot*> GridSlots;

	/** Array of slots that are affected by item hover, to clear it later. */
	UPROPERTY()
	TArray<UObsidianItemSlot_GridSlot*> AffectedGridSlots;

	UPROPERTY()
	TWeakObjectPtr<UObsidianInventoryItemsWidgetController> OwningWidgetController;

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCanvasPanel> Root_CanvasPanel;

private:
	void OffsetGridPositionByItemSpan(FIntPoint DraggedItemGridSpan, FIntPoint& OriginalPosition) const;
	
private:
	UPROPERTY()
	EObsidianGridOwner GridOwner = EObsidianGridOwner::None;

	UPROPERTY()
	FGameplayTag StashTag = FGameplayTag::EmptyTag;
};
