// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

// ~ Core
#include "CoreMinimal.h"

// ~ Project


#include "UI/Inventory/SubWidgets/ObsidianItemSlot.h"
#include "ObsidianItemSlot_Inventory.generated.h"

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianItemSlot_Inventory : public UObsidianItemSlot
{
	GENERATED_BODY()
	
public:
	void InitializeSlot(UObsidianInventoryGrid* InOwningInventory, const FVector2D& InSlotPosition);
	
	FVector2D GetSlotPosition() const
	{
		return SlotPosition;
	}
	
protected:
	virtual void NativeConstruct() override;
	
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	
protected:
	FVector2D SlotPosition;

	UPROPERTY()
	TObjectPtr<UObsidianInventoryGrid> OwningGrid;
};
