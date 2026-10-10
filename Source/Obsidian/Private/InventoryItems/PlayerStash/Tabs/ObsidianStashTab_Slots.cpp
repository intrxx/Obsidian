// Copyright 2026 out of sCope team - intrxx

#include "InventoryItems/PlayerStash/Tabs/ObsidianStashTab_Slots.h"

#include "InventoryItems/ObsidianInventoryItemDefinition.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"


UObsidianStashTab_Slots::UObsidianStashTab_Slots(const FObjectInitializer& InObjectInitializer)
	: Super(InObjectInitializer)
{
}

UObsidianInventoryItemInstance* UObsidianStashTab_Slots::GetInstanceAtPosition(const FObsidianItemPosition& InItemPosition)
{
	return SlotToItemMap.FindRef(InItemPosition.GetItemSlotTag());
}

TArray<FObsidianStashSlotDefinition> UObsidianStashTab_Slots::GetSlots() const
{
	return TabSlots;
}

bool UObsidianStashTab_Slots::DebugVerifyPositionFree(const FObsidianItemPosition& InPosition)
{
	return true; //TODO(intrxx) Implement
}

bool UObsidianStashTab_Slots::CanPlaceItemAtSpecificPosition(const FObsidianItemPosition& InSpecifiedPosition, const FGameplayTag& InItemCategory, const FGameplayTag& InItemBaseType, const FIntPoint& InItemGridSpan)
{
	const FObsidianStashSlotDefinition Slot = FindSlotByTag(InSpecifiedPosition.GetItemSlotTag());
	if(Slot.IsValid() == false)
	{
		return false;
	}
	
	return Slot.CanStashAtSlot(InItemCategory, InItemBaseType) == EObsidianPlacingAtSlotResult::CanPlace;
}

bool UObsidianStashTab_Slots::FindFirstAvailablePositionForItem(FObsidianItemPosition& OutFirstAvailablePosition, const FGameplayTag& InItemCategory, const FGameplayTag& InItemBaseType, const FIntPoint& InItemGridSpan)
{
	for (const FObsidianStashSlotDefinition& Slot : TabSlots)
	{
		if (Slot.CanStashAtSlot(InItemCategory, InItemBaseType) == EObsidianPlacingAtSlotResult::CanPlace)
		{
			OutFirstAvailablePosition = FObsidianItemPosition(Slot.GetStashSlotTag(), StashTabTag);
			return true;
		}
	}
	return false;
}

bool UObsidianStashTab_Slots::CanReplaceItemAtSpecificPosition(const FObsidianItemPosition& InSpecifiedPosition, const UObsidianInventoryItemInstance* InReplacingInstance)
{
	if (InReplacingInstance == nullptr)
	{
		return false;
	}
	
	return CheckReplacementPossible(InSpecifiedPosition, InReplacingInstance->GetItemCategoryTag(), InReplacingInstance->GetItemBaseTypeTag());
}

bool UObsidianStashTab_Slots::CanReplaceItemAtSpecificPosition(const FObsidianItemPosition& InSpecifiedPosition, const TSubclassOf<UObsidianInventoryItemDefinition>& InReplacingDef)
{
	if (InReplacingDef == nullptr)
	{
		return false;
	}

	const UObsidianInventoryItemDefinition* DefinitionDefault = InReplacingDef.GetDefaultObject();
	if (DefinitionDefault == nullptr)
	{
		return false;
	}
	
	return CheckReplacementPossible(InSpecifiedPosition, DefinitionDefault->GetItemCategoryTag(), DefinitionDefault->GetItemBaseTypeTag());
}

bool UObsidianStashTab_Slots::CheckReplacementPossible(const FObsidianItemPosition& InSpecifiedPosition, const FGameplayTag& InReplacingItemCategory, const FGameplayTag& InReplacingItemBaseType) const
{
	const FObsidianStashSlotDefinition Slot = FindSlotByTag(InSpecifiedPosition.GetItemSlotTag());
	if(Slot.IsValid() == false)
	{
		return false;
	}

	if (Slot.CanStashAtSlot(InReplacingItemCategory, InReplacingItemBaseType) != EObsidianPlacingAtSlotResult::CanPlace)
	{
		return false;
	}

	return true;
}

void UObsidianStashTab_Slots::MarkSpaceInTab(UObsidianInventoryItemInstance* InItemInstance, const FObsidianItemPosition& InAtPosition)
{
	ensureMsgf(!SlotToItemMap.Contains(InAtPosition.GetItemSlotTag()), TEXT("Item already exists in slot map at given position."));

	SlotToItemMap.Add(InAtPosition.GetItemSlotTag(), InItemInstance);
}

void UObsidianStashTab_Slots::UnmarkSpaceInTab(UObsidianInventoryItemInstance* InItemInstance, const FObsidianItemPosition& InAtPosition)
{
	ensureMsgf(SlotToItemMap.Contains(InAtPosition.GetItemSlotTag()), TEXT("Trying to remove item that does not exist in the slot map."));

	SlotToItemMap.Remove(InAtPosition.GetItemSlotTag());
}

void UObsidianStashTab_Slots::Construct(UObsidianPlayerStashComponent* InStashComponent)
{
	for (const FObsidianStashSlotDefinition& Slot : TabSlots)
	{
		
	}
	
	//TODO(intrxx) Get already added items, mark space
}

FObsidianStashSlotDefinition UObsidianStashTab_Slots::FindSlotByTag(const FGameplayTag& InSlotTag) const
{
	for(const FObsidianStashSlotDefinition& Slot : TabSlots)
	{
		if (Slot.GetStashSlotTag() == InSlotTag)
		{
			return Slot;
		}
	}

	return FObsidianStashSlotDefinition::InvalidSlot;
}




