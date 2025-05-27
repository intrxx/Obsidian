// Copyright 2024 out of sCope team - Michał Ogiński

#include "InventoryItems/Fragments/OInventoryItemFragment_Stacks.h"

// ~ Core

// ~ Project
#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "ObsidianTypes/ObsidianItemTypes.h"

void UOInventoryItemFragment_Stacks::OnInstancedCreated(UObsidianInventoryItemInstance* Instance) const
{
	for(const auto& Stack : InventoryItemStackNumbers)
	{
		Instance->AddItemStackCount(Stack.Key, Stack.Value);
	}
	Instance->SetStackable(bStackable);
}

int32 UOInventoryItemFragment_Stacks::GetItemStackNumberByTag(const FGameplayTag Tag) const
{
	if(const int32* StackPtr = InventoryItemStackNumbers.Find(Tag))
	{
		return *StackPtr;
	}
	
	return ObsidianDefaultStackCounts::GetUnifiedDefaultForTag(Tag);
}
