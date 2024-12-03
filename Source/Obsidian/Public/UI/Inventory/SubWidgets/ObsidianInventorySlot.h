// Copyright 2024 out of sCope team - Michał Ogiński

#pragma once

#include "CoreMinimal.h"
#include "UI/ObsidianWidgetBase.h"
#include "ObsidianInventorySlot.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnHoverOverSlotSignature, bool bEntered)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnMouseButtonDownOnSlotSignature, const FVector2D& SlotPosition)

class UImage;

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
	
	void SetSlotTaken();
	void SetSlotAvailable();
	void ResetSlot();

	FOnHoverOverSlotSignature OnHoverOverSlotDelegate;
	FOnMouseButtonDownOnSlotSignature OnMouseButtonDownOnSlotDelegate;

protected:
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

	virtual FReply NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	
	virtual void NativeConstruct() override;

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> ItemImage_Image;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Action_Image;

	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Setup")
	FSlateBrush SlotAvailableColor;
	
	UPROPERTY(EditDefaultsOnly, Category = "Obsidian|Setup")
	FSlateBrush SlotTakenColor;

	FVector2D SlotPosition;
};

