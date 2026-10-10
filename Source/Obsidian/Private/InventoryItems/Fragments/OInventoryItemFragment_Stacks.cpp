// Copyright 2026 out of sCope team - intrxx

#include "InventoryItems/Fragments/OInventoryItemFragment_Stacks.h"

#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "ObsidianTypes/ItemTypes/ObsidianItemTypes.h"


void UOInventoryItemFragment_Stacks::OnInstancedCreated(UObsidianInventoryItemInstance* InInstance) const
{
	for(const auto& Stack : InventoryItemStackNumbers)
	{
		InInstance->AddItemStackCount(Stack.Key, Stack.Value);
	}
	InInstance->SetStackable(bStackable);
}

int32 UOInventoryItemFragment_Stacks::GetItemStackNumberByTag(const FGameplayTag InTag) const
{
	if(const int32* StackPtr = InventoryItemStackNumbers.Find(InTag))
	{
		return *StackPtr;
	}
	
	return ObsidianDefaultStackCounts::GetUnifiedDefaultForTag(InTag);
}
