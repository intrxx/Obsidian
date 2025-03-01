// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "UI/ObsidianWidgetBase.h"
#include "ObsidianInventorySlot.generated.h"

class USizeBox;
class UObsidianInventorySlot;
class UImage;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnHoverOverSlotSignature, const bool bEntered, const UObsidianInventorySlot* AffectedSlot)
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnMouseButtonDownOnSlotSignature, const FVector2D& SlotPosition, const bool bShiftDown)

/**
 * 
 */
UCLASS()
class OBSIDIAN_API UObsidianInventorySlot : public UObsidianWidgetBase
{
	GENERATED_BODY()

public:
	void SetSlotPosition(const FVector2D& InSlotPosition)
	{
		SlotPosition = InSlotPosition;
	}

	FVector2D GetSlotPosition() const
	{
		return SlotPosition;
	}

	/** Sets the slot state based on bAvailable, if true sets it to green if false to red. */
	void SetSlotState(const bool bAvailable);
	void ResetSlot();

	FOnHoverOverSlotSignature OnHoverOverSlotDelegate;
	FOnMouseButtonDownOnSlotSignature OnMouseButtonDownOnSlotDelegate;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USizeBox> Root_SizeBox;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Slot_Image;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Action_Image;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Setup")
	FSlateBrush SlotAvailableColor;
	
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Setup")
	FSlateBrush SlotTakenColor;

	FVector2D SlotPosition;
};

