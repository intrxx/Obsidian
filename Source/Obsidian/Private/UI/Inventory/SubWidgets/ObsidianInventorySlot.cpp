// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/Inventory/SubWidgets/ObsidianInventorySlot.h"

void UObsidianInventorySlot::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	UE_LOG(LogTemp, Warning, TEXT("Hello mouse! Slot [%s] Here."), *GetNameSafe(this));
	
	OnHoverOverSlotDelegate.Broadcast();
	
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
}
