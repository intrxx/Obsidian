// Copyright 2026 out of sCope team - intrxx

#pragma once

#include <CoreMinimal.h>

#include "Obsidian/Public/UI/InventoryItems/Slots/ObsidianSlotBase.h"
#include "ObsidianSlot_GridSlot.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnGridSlotHoverSignature, UObsidianSlot_GridSlot* HoveredSlot, const bool bEntered);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnGridSlotPressedSignature, const UObsidianSlot_GridSlot* HoveredSlot,
	const FObsidianItemInteractionFlags& InteractionFlags);

/**
 * Grid Slot makes the Grid Panel.
 */
UCLASS()
class OBSIDIAN_API UObsidianSlot_GridSlot : public UObsidianSlotBase
{
	GENERATED_BODY()
	
public:
	void InitializeSlot(const FIntPoint& InGridSlotPosition);

	FIntPoint GetGridSlotPosition() const;
	
public:
	FOnGridSlotHoverSignature OnGridSlotHoverDelegate;
	FOnGridSlotPressedSignature OnGridSlotLeftButtonPressedDelegate;
	FOnGridSlotPressedSignature OnGridSlotRightButtonPressedDelegate;
	
protected:
	virtual void NativeConstruct() override;
	
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

private:
	FIntPoint GridSlotPosition = FIntPoint::NoneValue;
};
