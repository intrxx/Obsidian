// Copyright 2026 out of sCope team - intrxx

#include "InventoryItems/ObsidianInventoryItemDefinition.h"

// ~ Core

// ~ Project
#include "InventoryItems/ObsidianInventoryItemFragment.h"
#include "InventoryItems/Fragments/OInventoryItemFragment_Affixes.h"
#include "InventoryItems/Fragments/OInventoryItemFragment_Equippable.h"
#include "InventoryItems/Fragments/OInventoryItemFragment_Stacks.h"

DEFINE_LOG_CATEGORY(LogItems)

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

bool UObsidianInventoryItemDefinition::IsEquippable() const
{
	if(Cast<UOInventoryItemFragment_Equippable>(FindFragmentByClass(UOInventoryItemFragment_Equippable::StaticClass())))
	{
		return true;
	}
	return false;
}

bool UObsidianInventoryItemDefinition::DoesStartIdentified() const
{
	return bStartsIdentified;
}

bool UObsidianInventoryItemDefinition::DoesItemNeedsTwoSlots() const
{
	if(const FGameplayTagContainer* Container = ObsidianGameplayTags::GetSisterSlotAcceptedCategoriesMap().Find(ItemCategory))
	{
		return Container->IsEmpty();
	}
	return false;
}

FGameplayTag UObsidianInventoryItemDefinition::GetItemCategoryTag() const
{
	return ItemCategory;
}

FGameplayTag UObsidianInventoryItemDefinition::GetItemBaseTypeTag() const
{
	return ItemBaseType;
}

EObsidianItemRarity UObsidianInventoryItemDefinition::GetItemDefaultRarity() const
{
	return ItemDefaultRarity;
}

int32 UObsidianInventoryItemDefinition::GetItemLevel() const
{
#if !UE_BUILD_SHIPPING
	if (ItemLevel == INDEX_NONE)
	{
		UE_LOG(LogItems, Error, TEXT("Item [%s] has invalid Item Level."), *DebugName);
	}
#endif
	return ItemLevel;
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

