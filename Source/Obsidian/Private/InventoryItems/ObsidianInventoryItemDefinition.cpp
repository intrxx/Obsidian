// Copyright 2024 out of sCope team - Michał Ogiński


#include "InventoryItems/ObsidianInventoryItemDefinition.h"
#include "InventoryItems/ObsidianInventoryItemFragment.h"
#include "InventoryItems/Fragments/OInventoryItemFragment_Affixes.h"
#include "InventoryItems/Fragments/OInventoryItemFragment_Stacks.h"

UObsidianInventoryItemDefinition::UObsidianInventoryItemDefinition(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

bool UObsidianInventoryItemDefinition::HasStacks() const
{
	for(const UObsidianInventoryItemFragment* Fragment : ItemFragments)
	{
		if(Fragment->IsA(UOInventoryItemFragment_Stacks::StaticClass()))
		{
			return true;
		}
	}
	return false;
}

bool UObsidianInventoryItemDefinition::IsStackable() const
{
	const UOInventoryItemFragment_Stacks* StacksFrag = Cast<UOInventoryItemFragment_Stacks>(FindFragmentByClass(UOInventoryItemFragment_Stacks::StaticClass()));
	if(StacksFrag && StacksFrag->IsStackable())
	{
		return true;
	}
	return false;
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

FGameplayTag UObsidianInventoryItemDefinition::GetItemRarityTag() const
{
	if(const UOInventoryItemFragment_Affixes* AffixesFrag = Cast<UOInventoryItemFragment_Affixes>(FindFragmentByClass(UOInventoryItemFragment_Affixes::StaticClass())))
	{
		return AffixesFrag->GetItemRarityTag();
	}
	return ObsidianGameplayTags::Item_Rarity_Normal;
}

