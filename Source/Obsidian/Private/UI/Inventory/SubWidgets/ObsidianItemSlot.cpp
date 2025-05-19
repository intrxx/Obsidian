// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/Inventory/SubWidgets/ObsidianItemSlot.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"

void UObsidianItemSlot::NativeConstruct()
{
	Super::NativeConstruct();

	Action_Image->SetVisibility(ESlateVisibility::Hidden);
}

void UObsidianItemSlot::SetSlotAvailable(const bool bAvailable)
{
	if(bAvailable)
	{
		Action_Image->SetBrush(SlotGreenLightColor);
		Action_Image->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		Action_Image->SetBrush(SlotRedLightColor);
		Action_Image->SetVisibility(ESlateVisibility::Visible);
	}
}

void UObsidianItemSlot::ResetSlot()
{
	Action_Image->SetVisibility(ESlateVisibility::Hidden);
}


