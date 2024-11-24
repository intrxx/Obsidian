// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/Inventory/ObsidianItemWidget.h"

FReply UObsidianItemWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if(InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		UE_LOG(LogTemp, Warning, TEXT("You just clicked on an item!"));	
	}

	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}
