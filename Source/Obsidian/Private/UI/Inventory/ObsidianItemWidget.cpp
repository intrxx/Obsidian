// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/Inventory/ObsidianItemWidget.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"

FReply UObsidianItemWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	// InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton) - I don't know what is the difference, leaving it here for now
	if(InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		OnItemLeftMouseButtonPressedDelegate.Broadcast(ItemDesiredPosition);
	}

	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UObsidianItemWidget::InitializeItemWidget(const FVector2D& DesiredPosition, const FVector2D& ItemGridSpan, UTexture2D* ItemImage)
{
	Root_SizeBox->SetWidthOverride(ItemGridSpan.X * WidthConstant);
	Root_SizeBox->SetHeightOverride(ItemGridSpan.Y * HeightConstant);
	Item_Image->SetBrushFromTexture(ItemImage);

	ItemDesiredPosition = DesiredPosition;
}
