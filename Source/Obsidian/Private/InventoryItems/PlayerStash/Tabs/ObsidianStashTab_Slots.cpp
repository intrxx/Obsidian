// Copyright 2026 out of sCope team - intrxx

#include "InventoryItems/PlayerStash/Tabs/ObsidianStashTab_Slots.h"


#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "InventoryItems/ObsidianInventoryItemDefinition.h"

UObsidianStashTab_Slots::UObsidianStashTab_Slots(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UObsidianInventoryItemInstance* UObsidianStashTab_Slots::GetInstanceAtPosition(const FObsidianItemPosition& ItemPosition)
{
	return SlotToItemMap.FindRef(ItemPosition.GetItemSlotTag());
}

TArray<FObsidianStashSlotDefinition> UObsidianStashTab_Slots::GetSlots() const
{
	return TabSlots;
}

bool UObsidianStashTab_Slots::DebugVerifyPositionFree(const FObsidianItemPosition& Position)
{
	return true; //TODO(intrxx) Implement
}

bool UObsidianStashTab_Slots::CanPlaceItemAtSpecificPosition(const FObsidianItemPosition& SpecifiedPosition, const FGameplayTag& ItemCategory, const FGameplayTag& ItemBaseType, const FIntPoint& ItemGridSpan)
{
	const FObsidianStashSlotDefinition Slot = FindSlotByTag(SpecifiedPosition.GetItemSlotTag());
	if(Slot.IsValid() == false)
	{
		return false;
	}
	
	return Slot.CanStashAtSlot(ItemCategory, ItemBaseType) == EObsidianPlacingAtSlotResult::CanPlace;
}

bool UObsidianStashTab_Slots::FindFirstAvailablePositionForItem(FObsidianItemPosition& OutFirstAvailablePosition, const FGameplayTag& ItemCategory, const FGameplayTag& ItemBaseType, const FIntPoint& ItemGridSpan)
{
	for (const FObsidianStashSlotDefinition& Slot : TabSlots)
	{
		if (Slot.CanStashAtSlot(ItemCategory, ItemBaseType) == EObsidianPlacingAtSlotResult::CanPlace)
		{
			OutFirstAvailablePosition = FObsidianItemPosition(Slot.GetStashSlotTag(), StashTabTag);
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
	
	return CheckReplacementPossible(SpecifiedPosition, ReplacingInstance->GetItemCategoryTag(), ReplacingInstance->GetItemBaseTypeTag());
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
	
	return CheckReplacementPossible(SpecifiedPosition, DefinitionDefault->GetItemCategoryTag(), DefinitionDefault->GetItemBaseTypeTag());
}

bool UObsidianStashTab_Slots::CheckReplacementPossible(const FObsidianItemPosition& SpecifiedPosition, const FGameplayTag& ReplacingItemCategory, const FGameplayTag& ReplacingItemBaseType) const
{
	const FObsidianStashSlotDefinition Slot = FindSlotByTag(SpecifiedPosition.GetItemSlotTag());
	if(Slot.IsValid() == false)
	{
		return false;
	}

	if (Slot.CanStashAtSlot(ReplacingItemCategory, ReplacingItemBaseType) != EObsidianPlacingAtSlotResult::CanPlace)
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
	for (const FObsidianStashSlotDefinition& Slot : TabSlots)
	{
		
	}
	
	//TODO(intrxx) Get already added items, mark space
}

FObsidianStashSlotDefinition UObsidianStashTab_Slots::FindSlotByTag(const FGameplayTag& SlotTag) const
{
	for(const FObsidianStashSlotDefinition& Slot : TabSlots)
	{
		if (Slot.GetStashSlotTag() == SlotTag)
		{
			return Slot;
		}
	}

	return FObsidianStashSlotDefinition::InvalidSlot;
}




