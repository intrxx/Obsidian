// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/Inventory/ObsidianItemWidget.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"

FReply UObsidianItemWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if(InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		UE_LOG(LogTemp, Warning, TEXT("You just clicked on an item!"));	
	}

	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UObsidianItemWidget::InitializeItemWidget(const FVector2D& ItemGridSpan, UTexture2D* ItemImage) const
{
	Root_SizeBox->SetWidthOverride(ItemGridSpan.X * WidthConstant);
	Root_SizeBox->SetHeightOverride(ItemGridSpan.Y * HeightConstant);

	Item_Image->SetBrushFromTexture(ItemImage);
}
