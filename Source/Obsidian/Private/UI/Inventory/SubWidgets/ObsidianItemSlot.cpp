// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/Inventory/SubWidgets/ObsidianItemSlot.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"

void UObsidianItemSlot::NativeConstruct()
{
	Super::NativeConstruct();

	Action_Image->SetVisibility(ESlateVisibility::Hidden);
}

void UObsidianItemSlot::SetSlotState(const EObsidianItemSlotState InState) const
{
	if(InState == ISS_Neutral)
	{
		Action_Image->SetVisibility(ESlateVisibility::Hidden);
		return;
	}
	if(InState == ISS_GreenLight)
	{
		Action_Image->SetBrush(SlotGreenLightColor);
		Action_Image->SetVisibility(ESlateVisibility::Visible);
		return;
	}
	if(InState == ISS_RedLight)
	{
		Action_Image->SetBrush(SlotRedLightColor);
		Action_Image->SetVisibility(ESlateVisibility::Visible);
		return;
	}
	if(InState == ISS_Blocked)
	{
		Action_Image->SetBrush(SlotBlockedLightColor);
		Action_Image->SetVisibility(ESlateVisibility::Visible);
		return;
	}
}



