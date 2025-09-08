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

EObsidianItemSlotState UObsidianItemSlot::GetCurrentState() const
{
	return CurrentState;
}

void UObsidianItemSlot::SetSlotState(const EObsidianItemSlotState InState)
{
	if(InState == EObsidianItemSlotState::ISS_Neutral)
	{
		CurrentState = EObsidianItemSlotState::ISS_Neutral;
		Action_Image->SetVisibility(ESlateVisibility::Hidden);
		return;
	}
	if(InState == EObsidianItemSlotState::ISS_GreenLight)
	{
		CurrentState = EObsidianItemSlotState::ISS_GreenLight;
		Action_Image->SetBrush(SlotGreenLightColor);
		Action_Image->SetVisibility(ESlateVisibility::Visible);
		return;
	}
	if(InState == EObsidianItemSlotState::ISS_Selected)
	{
		CurrentState = EObsidianItemSlotState::ISS_Selected;
		Action_Image->SetBrush(SlotSelectedLightColor);
		Action_Image->SetVisibility(ESlateVisibility::Visible);
	}
	if(InState == EObsidianItemSlotState::ISS_RedLight)
	{
		CurrentState = EObsidianItemSlotState::ISS_RedLight;
		Action_Image->SetBrush(SlotRedLightColor);
		Action_Image->SetVisibility(ESlateVisibility::Visible);
		return;
	}
	if(InState == EObsidianItemSlotState::ISS_Blocked)
	{
		CurrentState = EObsidianItemSlotState::ISS_Blocked;
		Action_Image->SetBrush(SlotBlockedLightColor);
		Action_Image->SetVisibility(ESlateVisibility::Visible);
		return;
	}
}



