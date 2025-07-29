// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project


#include "Obsidian/Public/UI/Inventory/Slots/ObsidianItemSlot.h"
#include "ObsidianItemSlot_GridSlot.generated.h"

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianItemSlot_GridSlot : public UObsidianItemSlot
{
	GENERATED_BODY()
	
public:
	void InitializeSlot(UObsidianGrid* InOwningGrid, const FIntPoint& InSlotGridPosition);
	
	FIntPoint GetSlotPositionOnGrid() const
	{
		return GridPosition;
	}
	
protected:
	virtual void NativeConstruct() override;
	
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	
protected:
	FIntPoint GridPosition;

	UPROPERTY()
	TObjectPtr<UObsidianGrid> OwningGrid;
};
