// Copyright 2024 out of sCope team - Michał Ogiński

#include "InventoryItems/ObsidianItemsFunctionLibrary.h"

// ~ Core

// ~ Project
#include "InventoryItems/ObsidianInventoryItemDefinition.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "InventoryItems/ObsidianInventoryItemFragment.h"
#include "InventoryItems/Fragments/OInventoryItemFragment_Appearance.h"
#include "InventoryItems/Fragments/OInventoryItemFragment_Stacks.h"

const UObsidianInventoryItemFragment* UObsidianItemsFunctionLibrary::FindItemDefinitionFragment(const TSubclassOf<UObsidianInventoryItemDefinition> ItemDef, const TSubclassOf<UObsidianInventoryItemFragment> FragmentClass)
{
	if((ItemDef != nullptr) && (FragmentClass != nullptr))
	{
		return GetDefault<UObsidianInventoryItemDefinition>(ItemDef)->FindFragmentByClass(FragmentClass);
	}
	return nullptr;
}

bool UObsidianItemsFunctionLibrary::IsTheSameItem(const UObsidianInventoryItemInstance* InstanceA, const UObsidianInventoryItemInstance* InstanceB)
{
	if(InstanceA == nullptr || InstanceB == nullptr)
	{
		return false;
	}

	if(InstanceA->GetItemDef().Get() == InstanceB->GetItemDef().Get())
	{
		return true;
	}
	return false;
}

bool UObsidianItemsFunctionLibrary::IsTheSameItem_WithDef(const UObsidianInventoryItemInstance* Instance, const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef)
{
	if(Instance == nullptr || ItemDef == nullptr)
	{
		return false;
	}

	if(Instance->GetItemDef().Get() == ItemDef.Get())
	{
		return true;
	}
	return false;
}

bool UObsidianItemsFunctionLibrary::GetItemStatsForItemInstance(const UObsidianInventoryItemInstance* ItemInstance, FObsidianItemStats& OutItemStats)
{
	if(ItemInstance == nullptr)
	{
		return false;
	}
	
	if(ItemInstance->IsStackable())
	{
		OutItemStats.SetStacks(ItemInstance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current),
			 ItemInstance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Max));
	}

	OutItemStats.SetItemImage(ItemInstance->GetItemImage(), ItemInstance->GetItemGridSpan());
	OutItemStats.SetDisplayName(ItemInstance->GetItemDisplayName());
	OutItemStats.SetDescription(ItemInstance->GetItemDescription());
	OutItemStats.SetAdditionalDescription(ItemInstance->GetItemAdditionalDescription());

	OutItemStats.ItemRarity = ItemInstance->GetItemRarity();
	
	const bool bIdentified = ItemInstance->IsItemIdentified();
	OutItemStats.SetIdentified(bIdentified);
	if(bIdentified)
	{
		OutItemStats.SetAffixDescriptionRows(ItemInstance->GetAffixesAsUIDescription());
	}

	return true;
}

bool UObsidianItemsFunctionLibrary::GetItemStatsForItemDefinition(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef, const int32 TemplateCurrentItemStacks, FObsidianItemStats& OutItemStats)
{
	if(IsValid(ItemDef) == false)
	{
		return false;
	}

	const UObsidianInventoryItemDefinition* ItemDefault = GetDefault<UObsidianInventoryItemDefinition>(ItemDef);
	if(ItemDefault == nullptr)
	{
		return false;
	}
	
	if(ItemDefault->IsStackable())
	{
		if(const UOInventoryItemFragment_Stacks* StacksFrag = Cast<UOInventoryItemFragment_Stacks>(ItemDefault->FindFragmentByClass(UOInventoryItemFragment_Stacks::StaticClass())))
		{
			OutItemStats.SetStacks(TemplateCurrentItemStacks, // Current Item Stacks are not present on ItemDef, they are directly on Pickable Item
				 StacksFrag->GetItemStackNumberByTag(ObsidianGameplayTags::Item_StackCount_Max));
		}
	}

	if(const UOInventoryItemFragment_Appearance* AppearanceFrag = Cast<UOInventoryItemFragment_Appearance>(ItemDefault->FindFragmentByClass(UOInventoryItemFragment_Appearance::StaticClass())))
	{
		OutItemStats.SetItemImage(AppearanceFrag->GetItemImage(), AppearanceFrag->GetItemGridSpanFromDesc());
		OutItemStats.SetDisplayName(AppearanceFrag->GetItemDisplayName());
		OutItemStats.SetDescription(AppearanceFrag->GetItemDescription());
		OutItemStats.SetAdditionalDescription(AppearanceFrag->GetItemAdditionalDescription());
	}

	if(const UOInventoryItemFragment_Affixes* AffixesFrag = Cast<UOInventoryItemFragment_Affixes>(ItemDefault->FindFragmentByClass(UOInventoryItemFragment_Affixes::StaticClass())))
	{
		OutItemStats.ItemRarity = AffixesFrag->GetItemRarityTag();
		const bool bIdentified = AffixesFrag->IsItemIdentified();
		OutItemStats.SetIdentified(bIdentified);
		if(bIdentified)
		{
			OutItemStats.SetAffixDescriptionRows(AffixesFrag->GetAffixesAsUIDescription());
		}
	}

	return true;
}

