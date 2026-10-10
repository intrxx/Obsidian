// Copyright 2026 out of sCope - intrxx

#include "InventoryItems/Fragments/Shards/ObsidianUsableShard_OrbOfRescription.h"

#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "InventoryItems/ObsidianItemsFunctionLibrary.h"
#include "Obsidian/ObsidianLogCategories.h"


bool UObsidianUsableShard_OrbOfRescription::OnItemUsed(AObsidianPlayerController* InItemOwner,
	UObsidianInventoryItemInstance* InUsingInstance, UObsidianInventoryItemInstance* InUsingOntoInstance)
{
	if(InItemOwner && InUsingInstance && InUsingOntoInstance)
	{
		if(CanUseOnItem(InUsingOntoInstance))
		{
			const FObsidianDynamicItemAffix SkillImplicitToAdd = UObsidianItemsFunctionLibrary::GetRandomSkillImplicitForItem(
					InUsingOntoInstance);
			if (SkillImplicitToAdd && InUsingOntoInstance->RemoveSkillImplicitAffix())
			{
				FObsidianActiveItemAffix ActiveAffix;
				ActiveAffix.InitializeWithDynamic(SkillImplicitToAdd, InUsingOntoInstance->GetItemLevel());
				InUsingOntoInstance->AddAffix(ActiveAffix);
				return true;
			}
		}
		
		UE_LOG(ObLogCrafting, Warning, TEXT("Orb Of Rescription could not be used on provided [%s] Instance. "
									"Or Skill Implicit could not be replaced with valid one (could not remove the skill "
									" implicit or/and find a replacement.)"),
										*GetNameSafe(InUsingOntoInstance));
	}
	return false;
}

void UObsidianUsableShard_OrbOfRescription::OnItemUsed_UIContext(const TArray<UObsidianInventoryItemInstance*>& InAllItems,
	FObsidianItemsMatchingUsableContext& OutItemsMatchingContext)
{
	for(const UObsidianInventoryItemInstance* Instance : InAllItems)
	{
		if(CanUseOnItem(Instance))
		{
			OutItemsMatchingContext.AddMatchingItem(Instance);
		}
	}
}

bool UObsidianUsableShard_OrbOfRescription::CanUseOnItem(const UObsidianInventoryItemInstance* InInstance) const
{
	if (InInstance == nullptr)
	{
		return false;
	}

	return InInstance->IsItemEquippable() &&
			InInstance->IsItemIdentified() &&
			InInstance->IsUniqueOrSet() == false &&
			InInstance->HasSkillImplicitAffix();	
}
