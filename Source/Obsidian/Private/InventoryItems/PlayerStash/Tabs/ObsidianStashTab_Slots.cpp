// Copyright 2024 out of sCope team - Michał Ogiński

#include "InventoryItems/PlayerStash/Tabs/ObsidianStashTab_Slots.h"

// ~ Core

// ~ Project
UObsidianStashTab_Slots::UObsidianStashTab_Slots(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UObsidianStashTab_Slots::MarkSpaceInTab(UObsidianInventoryItemInstance* ItemInstance, const FObsidianItemPosition& AtPosition)
{
	ensureMsgf(!SlotToItemMap.Contains(AtPosition.GetItemSlotTag()), TEXT("Item already exists in slot map at given position."));

	SlotToItemMap.Add(AtPosition.GetItemSlotTag(), ItemInstance);
}

void UObsidianStashTab_Slots::UnmarkSpaceInTab(UObsidianInventoryItemInstance* ItemInstance, const FObsidianItemPosition& AtPosition)
{
	ensureMsgf(SlotToItemMap.Contains(AtPosition.GetItemSlotTag()), TEXT("Trying to remove item that does not exist in the slot map."));

	SlotToItemMap.Remove(AtPosition.GetItemSlotTag());
}
