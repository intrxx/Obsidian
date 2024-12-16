// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/Inventory/SubWidgets/ObsidianInventorySlot.h"

#include "Components/Image.h"

void UObsidianInventorySlot::NativeConstruct()
{
	Super::NativeConstruct();

	Action_Image->SetVisibility(ESlateVisibility::Hidden);
}

void UObsidianInventorySlot::SetSlotState(bool bAvailable)
{
	if(bAvailable)
	{
		Action_Image->SetBrush(SlotAvailableColor);
		Action_Image->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		Action_Image->SetBrush(SlotTakenColor);
		Action_Image->SetVisibility(ESlateVisibility::Visible);
	}
}

void UObsidianInventorySlot::ResetSlot()
{
	Action_Image->SetVisibility(ESlateVisibility::Hidden);
}

void UObsidianInventorySlot::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	UE_LOG(LogTemp, Warning, TEXT("Hello mouse! Slot [%s] Here."), *GetNameSafe(this));
	
	OnHoverOverSlotDelegate.Broadcast(true, this);
	
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
}

void UObsidianInventorySlot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	OnHoverOverSlotDelegate.Broadcast(false, this);
	
	Super::NativeOnMouseLeave(InMouseEvent);
}

FReply UObsidianInventorySlot::NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if(InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		UE_LOG(LogTemp, Warning, TEXT("SLOT!"));
		OnMouseButtonDownOnSlotDelegate.Broadcast(SlotPosition);
	}
	
	return Super::NativeOnPreviewMouseButtonDown(InGeometry, InMouseEvent);
}

