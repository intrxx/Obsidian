// Copyright 2024 out of sCope team - Michał Ogiński


#include "UI/Inventory/SubWidgets/ObsidianItemSlot_Equipment.h"

void UObsidianItemSlot_Equipment::InitializeSlot(UObsidianInventory* InOwningInventory, const FGameplayTag& InSlotTag)
{
	OwningInventory = InOwningInventory;
	
	if(!SlotTag.IsValid()) // Slot Tag has been already set in Blueprint
	{
		SlotTag = InSlotTag;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Attempting to set SlotTag but it has already been set."));
	}
}
