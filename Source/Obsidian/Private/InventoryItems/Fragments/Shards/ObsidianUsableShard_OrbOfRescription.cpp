// Copyright 2026 out of sCope - intrxx

#include "InventoryItems/Fragments/Shards/ObsidianUsableShard_OrbOfRescription.h"

#include "InventoryItems/ObsidianInventoryItemInstance.h"
#include "InventoryItems/ObsidianItemsFunctionLibrary.h"
#include "Obsidian/ObsidianGameModule.h"


bool UObsidianUsableShard_OrbOfRescription::OnItemUsed(AObsidianPlayerController* ItemOwner,
	UObsidianInventoryItemInstance* UsingInstance, UObsidianInventoryItemInstance* UsingOntoInstance)
{
	if(ItemOwner && UsingInstance && UsingOntoInstance)
	{
		if(CanUseOnItem(UsingOntoInstance))
		{
			const FObsidianDynamicItemAffix SkillImplicitToAdd = UObsidianItemsFunctionLibrary::GetRandomSkillImplicitForItem(
					UsingOntoInstance);
			if (SkillImplicitToAdd && UsingOntoInstance->RemoveSkillImplicitAffix())
			{
				FObsidianActiveItemAffix ActiveAffix;
				ActiveAffix.InitializeWithDynamic(SkillImplicitToAdd, UsingOntoInstance->GetItemLevel());
				UsingOntoInstance->AddAffix(ActiveAffix);
				return true;
			}
		}
		
		UE_LOG(LogObsidian, Warning, TEXT("Orb Of Rescription could not be used on provided [%s] Instance. "
									"Or Skill Implicit could not be replaced with valid one (could not remove the skill "
									" implicit or/and find a replacement.)"),
										*GetNameSafe(UsingOntoInstance));
	}
	return false;
}

void UObsidianUsableShard_OrbOfRescription::OnItemUsed_UIContext(const TArray<UObsidianInventoryItemInstance*>& AllItems,
	FObsidianItemsMatchingUsableContext& OutItemsMatchingContext)
{
	for(const UObsidianInventoryItemInstance* Instance : AllItems)
	{
		if(CanUseOnItem(Instance))
		{
			OutItemsMatchingContext.AddMatchingItem(Instance);
		}
	}
}

bool UObsidianUsableShard_OrbOfRescription::CanUseOnItem(const UObsidianInventoryItemInstance* Instance) const
{
	if (Instance == nullptr)
	{
		return false;
	}

	return Instance->IsItemEquippable() &&
			Instance->IsItemIdentified() &&
			Instance->IsUniqueOrSet() == false &&
			Instance->HasSkillImplicitAffix();	
}
