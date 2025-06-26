// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project


#include "Obsidian/Public/UI/Inventory/Slots/ObsidianItemSlot.h"
#include "ObsidianItemSlot_Inventory.generated.h"

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianItemSlot_Inventory : public UObsidianItemSlot
{
	GENERATED_BODY()
	
public:
	void InitializeSlot(UObsidianInventoryGrid* InOwningInventory, const FIntPoint& InSlotPosition);
	
	FIntPoint GetSlotPosition() const
	{
		return SlotPosition;
	}
	
protected:
	virtual void NativeConstruct() override;
	
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	
protected:
	FIntPoint SlotPosition;

	UPROPERTY()
	TObjectPtr<UObsidianInventoryGrid> OwningGrid;
};
