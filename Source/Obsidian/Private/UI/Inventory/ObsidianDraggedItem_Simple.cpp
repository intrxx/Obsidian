// Copyright 2024 out of sCope team - Michał Ogiński

#include "UI/Inventory/ObsidianDraggedItem_Simple.h"

// ~ Core
#include "Components/SizeBox.h"
#include "Components/Image.h"

// ~ Project
#include "ObsidianTypes/ObsidianItemTypes.h"

void UObsidianDraggedItem_Simple::NativeConstruct()
{
	Super::NativeConstruct();
	
	SetVisibility(ESlateVisibility::HitTestInvisible);
	SetRenderOpacity(0.9f);
}

void UObsidianDraggedItem_Simple::InitializeDraggedItem(const FSlateBrush& InItemImage, const FVector2D& InItemGridSpan)
{
	if(Item_Image)
	{
		Item_Image->SetBrush(InItemImage);
	}

	if(Root_SizeBox)
	{
		const float WidgetOverride = (InItemGridSpan.X * ObsidianInventoryItemsStatics::InventorySlotSize.X) * ItemDownsizeScale;
		Root_SizeBox->SetWidthOverride(WidgetOverride);
		
		const float HeightOverride = (InItemGridSpan.Y * ObsidianInventoryItemsStatics::InventorySlotSize.Y) * ItemDownsizeScale;
		Root_SizeBox->SetHeightOverride(HeightOverride);
	}
}

FVector2D UObsidianDraggedItem_Simple::GetItemSize() const
{
	if(Root_SizeBox)
	{
		return FVector2D(Root_SizeBox->GetWidthOverride(), Root_SizeBox->GetHeightOverride());
	}
	return FVector2D::ZeroVector;
}


