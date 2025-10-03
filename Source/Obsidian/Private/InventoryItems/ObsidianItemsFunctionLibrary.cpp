// Copyright 2024 out of sCope team - Michał Ogiński

#include "InventoryItems/ObsidianItemsFunctionLibrary.h"

// ~ Core

// ~ Project
#include "InventoryItems/ObsidianInventoryItemDefinition.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "InventoryItems/ObsidianInventoryItemFragment.h"
#include "InventoryItems/Fragments/OInventoryItemFragment_Appearance.h"
#include "InventoryItems/Fragments/OInventoryItemFragment_Stacks.h"
#include "InventoryItems/Inventory/ObsidianInventoryComponent.h"
#include "InventoryItems/PlayerStash/ObsidianPlayerStashComponent.h"

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

bool UObsidianItemsFunctionLibrary::GetItemStats(const UObsidianInventoryItemInstance* ItemInstance, FObsidianItemStats& OutItemStats)
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

bool UObsidianItemsFunctionLibrary::GetItemStats_WithDef(const TSubclassOf<UObsidianInventoryItemDefinition>& ItemDef, const int32 TemplateCurrentItemStacks, FObsidianItemStats& OutItemStats)
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
		const bool bIdentified = AffixesFrag->DoesStartIdentified();
		OutItemStats.SetIdentified(bIdentified);
		if(bIdentified)
		{
			//TODO(intrxx) #AffixRefactor
			//OutItemStats.SetAffixDescriptionRows(AffixesFrag->GetAffixesAsUIDescription());
		}
	}

	return true;
}

int32 UObsidianItemsFunctionLibrary::GetAmountOfStacksAllowedToAddToItem(const AActor* Owner, const UObsidianInventoryItemInstance* AddingFromInstance, const UObsidianInventoryItemInstance* InstanceToAddTo)
{
	if(Owner == nullptr)
	{
		UE_LOG(LogInventory, Error, TEXT("Owner is nullptr in [%hs]"), ANSI_TO_TCHAR(__FUNCTION__));
		return 0; 
	}
	
	const int32 CurrentStackCount = InstanceToAddTo->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current);
	if(CurrentStackCount == 0)
	{
		return 0;
	}

	int32 CombinedStacks = 0;
	const int32 LimitStackCount = InstanceToAddTo->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Limit);
	if(LimitStackCount > 0)
	{
		int32 StacksInInventory = 0;
		int32 StacksInStash = 0;
		if(UObsidianInventoryComponent* InventoryComp = UObsidianInventoryComponent::FindInventoryComponent(Owner))
		{
			StacksInInventory = InventoryComp->FindAllStacksForGivenItem(AddingFromInstance);
		}
		if(UObsidianPlayerStashComponent* PlayerStashComp = UObsidianPlayerStashComponent::FindPlayerStashComponent(Owner))
		{
			StacksInStash = PlayerStashComp->FindAllStacksForGivenItem(AddingFromInstance);
		}

		CombinedStacks = StacksInInventory + StacksInStash;
		checkf(CombinedStacks <= LimitStackCount, TEXT("Combined Stacks of held item is already bigger than Stacks Limit for this item, something went wrong."));
		if((CombinedStacks == 1) || (CombinedStacks == LimitStackCount))
		{
			return 0;
		}
	}
	
	const int32 AddingFromInstanceCurrentStacks = AddingFromInstance->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current);
	const int32 StacksThatCanBeAddedToInventory = LimitStackCount == 0 ? AddingFromInstanceCurrentStacks : LimitStackCount - CombinedStacks;
	if(StacksThatCanBeAddedToInventory <= 0)
	{
		return 0;
	}
			
	const int32 MaxStackCount = InstanceToAddTo->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Max);
	const int32 AmountThatCanBeAddedToInstance = FMath::Clamp<int32>((MaxStackCount - CurrentStackCount), 0, StacksThatCanBeAddedToInventory);
	return FMath::Min<int32>(AmountThatCanBeAddedToInstance, AddingFromInstanceCurrentStacks);
}

int32 UObsidianItemsFunctionLibrary::GetAmountOfStacksAllowedToAddToItem_WithDef(const AActor* Owner, const TSubclassOf<UObsidianInventoryItemDefinition>& AddingFromItemDef, const int32 AddingFromItemDefCurrentStacks, const UObsidianInventoryItemInstance* InstanceToAddTo)
{
	const int32 CurrentStackCount = InstanceToAddTo->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Current);
	if(CurrentStackCount == 0)
	{
		return 0;
	}

	int32 CombinedStacks = 0;
	const int32 LimitStackCount = InstanceToAddTo->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Limit);
	if(LimitStackCount > 0)
	{
		int32 StacksInInventory = 0;
		int32 StacksInStash = 0;
		if(UObsidianInventoryComponent* InventoryComp = UObsidianInventoryComponent::FindInventoryComponent(Owner))
		{
			StacksInInventory = InventoryComp->FindAllStacksForGivenItem(AddingFromItemDef);
		}
		if(UObsidianPlayerStashComponent* PlayerStashComp = UObsidianPlayerStashComponent::FindPlayerStashComponent(Owner))
		{
			StacksInStash = PlayerStashComp->FindAllStacksForGivenItem(AddingFromItemDef);
		}

		CombinedStacks = StacksInInventory + StacksInStash;
		checkf(CombinedStacks <= LimitStackCount, TEXT("Combined Stacks of held item is already bigger than Stacks Limit for this item, something went wrong."));
		if((CombinedStacks == 1) || (CombinedStacks == LimitStackCount))
		{
			return 0;
		}
	}
			
	const int32 StacksThatCanBeAddedToInventory = LimitStackCount == 0 ? AddingFromItemDefCurrentStacks : LimitStackCount - CombinedStacks;
	if(StacksThatCanBeAddedToInventory <= 0)
	{
		return 0;
	}
			
	const int32 MaxStackCount = InstanceToAddTo->GetItemStackCount(ObsidianGameplayTags::Item_StackCount_Max);
	const int32 AmountThatCanBeAddedToInstance = FMath::Clamp((MaxStackCount - CurrentStackCount), 0, StacksThatCanBeAddedToInventory);
	return FMath::Min(AmountThatCanBeAddedToInstance, AddingFromItemDefCurrentStacks);
}

bool UObsidianItemsFunctionLibrary::IsItemUnique(const UObsidianInventoryItemInstance* ItemInstance)
{
	return ItemInstance->GetItemRarity() == ObsidianGameplayTags::Item_Rarity_Unique;
}

FGameplayTag UObsidianItemsFunctionLibrary::GetCategoryTagFromDraggedItem(const FDraggedItem& DraggedItem)
{
	if (DraggedItem.IsEmpty())
	{
		return FGameplayTag::EmptyTag;
	}

	if (const UObsidianInventoryItemInstance* DraggedInstance = DraggedItem.Instance)
	{
		return DraggedInstance->GetItemCategoryTag();
	}

	if (const TSubclassOf<UObsidianInventoryItemDefinition> DraggedItemDef = DraggedItem.ItemDef)
	{
		if (const UObsidianInventoryItemDefinition* ItemDefault = GetDefault<UObsidianInventoryItemDefinition>(DraggedItemDef))
		{
			return ItemDefault->GetItemCategoryTag();
		}
	}
	
	return FGameplayTag::EmptyTag;
}

