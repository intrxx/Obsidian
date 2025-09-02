// Copyright 2024 out of sCope team - Michał Ogiński

#include "InventoryItems/PlayerStash/Tabs/ObsidianStashTab_Slots.h"

// ~ Core

// ~ Project
#include "InventoryItems/ObsidianInventoryItemInstance.h"

UObsidianStashTab_Slots::UObsidianStashTab_Slots(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UObsidianInventoryItemInstance* UObsidianStashTab_Slots::GetInstanceAtPosition(const FObsidianItemPosition& ItemPosition)
{
	return SlotToItemMap.FindRef(ItemPosition.GetItemSlotTag());
}

bool UObsidianStashTab_Slots::DebugVerifyPositionFree(const FObsidianItemPosition& Position)
{
	return true; //TODO Implement
}

bool UObsidianStashTab_Slots::CanPlaceItemAtSpecificPosition(const FObsidianItemPosition& SpecifiedPosition, const FGameplayTag& ItemCategory, const FIntPoint& ItemGridSpan)
{
	const FObsidianSlotDefinition Slot = FindSlotByTag(SpecifiedPosition.GetItemSlotTag());
	if(Slot.IsValid() == false)
	{
		return false;
	}
	
	return Slot.CanPlaceAtSlot(ItemCategory) == EObsidianEquipCheckResult::CanEquip;
}

bool UObsidianStashTab_Slots::FindFirstAvailablePositionForItem(FObsidianItemPosition& OutFirstAvailablePosition, const FGameplayTag& ItemCategory, const FIntPoint& ItemGridSpan)
{
	for (const FObsidianSlotDefinition& Slot : TabSlots)
	{
		if (Slot.CanPlaceAtSlot(ItemCategory) == EObsidianEquipCheckResult::CanEquip)
		{
			OutFirstAvailablePosition = Slot.GetSlotTag();
			return true;
		}
	}
	return false;
}

bool UObsidianStashTab_Slots::CanReplaceItemAtSpecificPosition(const FObsidianItemPosition& SpecifiedPosition, const UObsidianInventoryItemInstance* ReplacingInstance)
{
	if (ReplacingInstance == nullptr)
	{
		return false;
	}
	
	if(ReplacingInstance->IsItemEquippable() == false)
	{
		return false;
	}
	
	if(ReplacingInstance->IsItemIdentified() == false)
	{
		return false;
	}
	
	return CheckReplacementPossible(SpecifiedPosition, ReplacingInstance->GetItemCategoryTag());
}

bool UObsidianStashTab_Slots::CanReplaceItemAtSpecificPosition(const FObsidianItemPosition& SpecifiedPosition, const TSubclassOf<UObsidianInventoryItemDefinition>& ReplacingDef)
{
	if (ReplacingDef == nullptr)
	{
		return false;
	}

	const UObsidianInventoryItemDefinition* DefinitionDefault = ReplacingDef.GetDefaultObject();
	if (DefinitionDefault == nullptr)
	{
		return false;
	}
	
	if(DefinitionDefault->IsEquippable() == false)
	{
		return false;
	}
	
	if(DefinitionDefault->IsIdentified() == false)
	{
		return false;
	}
	
	return CheckReplacementPossible(SpecifiedPosition, DefinitionDefault->GetItemCategoryTag());
}

bool UObsidianStashTab_Slots::CheckReplacementPossible(const FObsidianItemPosition& SpecifiedPosition, const FGameplayTag& ReplacingItemCategory) const
{
	const FObsidianSlotDefinition Slot = FindSlotByTag(SpecifiedPosition.GetItemSlotTag());
	if(Slot.IsValid() == false)
	{
		return false;
	}

	if (Slot.CanPlaceAtSlot(ReplacingItemCategory) != EObsidianEquipCheckResult::CanEquip)
	{
		return false;
	}

	return true;
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

void UObsidianStashTab_Slots::Construct(UObsidianPlayerStashComponent* StashComponent)
{
	for (const FObsidianSlotDefinition& Slot : TabSlots)
	{
		
	}
	
	//TODO Get already added items, mark space
}

FObsidianSlotDefinition UObsidianStashTab_Slots::FindSlotByTag(const FGameplayTag& SlotTag) const
{
	for(const FObsidianSlotDefinition& Slot : TabSlots)
	{
		if (Slot.GetSlotTag() == SlotTag)
		{
			return Slot;
		}
	}

	return FObsidianSlotDefinition::InvalidSlot;
}




