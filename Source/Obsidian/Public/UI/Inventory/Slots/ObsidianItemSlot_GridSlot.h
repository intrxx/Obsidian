// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include <CoreMinimal.h>

#include "Obsidian/Public/UI/Inventory/Slots/ObsidianItemSlot.h"
#include "ObsidianItemSlot_GridSlot.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnGridSlotHoverSignature, UObsidianItemSlot_GridSlot* HoveredSlot, const bool bEntered);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnGridSlotPressedSignature, const UObsidianItemSlot_GridSlot* HoveredSlot,
	const FObsidianItemInteractionFlags& InteractionFlags);

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianItemSlot_GridSlot : public UObsidianItemSlot
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
