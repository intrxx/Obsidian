// Copyright 2024 out of sCope team - Michał Ogiński

#include "Obsidian/Public/UI/Inventory/Slots/ObsidianItemSlot.h"

// ~ Core
#include "Components/Image.h"
#include "Components/SizeBox.h"

// ~ Project

void UObsidianItemSlot::NativeConstruct()
{
	Super::NativeConstruct();

	Action_Image->SetVisibility(ESlateVisibility::Hidden);
}

void UObsidianItemSlot::SetSlotState(const EObsidianItemSlotState InState, const EObsidianItemSlotStatePriority InPriority)
{
	if (CurrentStatePriority > InPriority)
	{
		return;
	}
	
	if(InState == EObsidianItemSlotState::Neutral)
	{
		CurrentStatePriority = EObsidianItemSlotStatePriority::Low;
		Action_Image->SetVisibility(ESlateVisibility::Hidden);
		return;
	}
	if(InState == EObsidianItemSlotState::GreenLight)
	{
		CurrentStatePriority = InPriority;
		Action_Image->SetBrush(SlotGreenLightColor);
		Action_Image->SetVisibility(ESlateVisibility::Visible);
		return;
	}
	if(InState == EObsidianItemSlotState::Selected)
	{
		CurrentStatePriority = InPriority;
		Action_Image->SetBrush(SlotSelectedLightColor);
		Action_Image->SetVisibility(ESlateVisibility::Visible);
	}
	if(InState == EObsidianItemSlotState::RedLight)
	{
		CurrentStatePriority = InPriority;
		Action_Image->SetBrush(SlotRedLightColor);
		Action_Image->SetVisibility(ESlateVisibility::Visible);
		return;
	}
	if(InState == EObsidianItemSlotState::Blocked)
	{
		CurrentStatePriority = InPriority;
		Action_Image->SetBrush(SlotBlockedLightColor);
		Action_Image->SetVisibility(ESlateVisibility::Visible);
		return;
	}
}



