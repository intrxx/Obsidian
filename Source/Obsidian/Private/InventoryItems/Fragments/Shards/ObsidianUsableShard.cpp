// Copyright 2026 out of sCope team - intrxx

#include "InventoryItems/Fragments/Shards/ObsidianUsableShard.h"

#include "InventoryItems/ObsidianInventoryItemInstance.h"

// ~ Start of FObsidianItemsMatchingUsableContext

void FObsidianItemsMatchingUsableContext::AddMatchingItem(const UObsidianInventoryItemInstance* InstanceToAdd)
{
	if (InstanceToAdd == nullptr)
	{
		return;
	}
	
	const FObsidianItemPosition CurrentPosition = InstanceToAdd->GetItemCurrentPosition();

	if (CurrentPosition.IsOnStash())
	{
		StashItemsMatchingContext.Add(CurrentPosition.GetOwningStashTabTag(), CurrentPosition);
	}
	else if(CurrentPosition.IsOnInventoryGrid())
	{
		InventoryItemsMatchingContext.Add(CurrentPosition);
	}
	else if(CurrentPosition.IsOnEquipmentSlot())
	{
		EquipmentItemsMatchingContext.Add(CurrentPosition);
	}
}

bool FObsidianItemsMatchingUsableContext::HasAnyMatchingItems() const
{
	return !StashItemsMatchingContext.IsEmpty() ||
		!InventoryItemsMatchingContext.IsEmpty() ||
		!EquipmentItemsMatchingContext.IsEmpty();
}

// ~ End of FObsidianItemsMatchingUsableContext

bool UObsidianUsableShard::OnItemUsed(AObsidianPlayerController* ItemOwner, UObsidianInventoryItemInstance* UsingInstance,
                                      UObsidianInventoryItemInstance* UsingOntoInstance)
{
	return false;
}

void UObsidianUsableShard::OnItemUsed_UIContext(const TArray<UObsidianInventoryItemInstance*>& AllItems,
	FObsidianItemsMatchingUsableContext& OutItemsMatchingContext)
{
}
