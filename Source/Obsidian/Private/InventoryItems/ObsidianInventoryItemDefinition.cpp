// Copyright 2024 out of sCope team - Michał Ogiński


#include "InventoryItems/ObsidianInventoryItemDefinition.h"
#include "InventoryItems/ObsidianInventoryItemFragment.h"

UObsidianInventoryItemDefinition::UObsidianInventoryItemDefinition(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

const UObsidianInventoryItemFragment* UObsidianInventoryItemDefinition::FindFragmentByClass(const TSubclassOf<UObsidianInventoryItemFragment>& FragmentClass) const
{
	if(FragmentClass == nullptr)
	{
		return nullptr;
	}

	for(const UObsidianInventoryItemFragment* Fragment : ItemFragments)
	{
		if(Fragment->IsA(FragmentClass))
		{
			return Fragment;
		}
	}

	return nullptr;
}
