// Copyright 2024 out of sCope team - Michał Ogiński

#include "InventoryItems/Fragments/OInventoryItemFragment_Affixes.h"

// ~ Core

// ~ Project
#include "InventoryItems/ObsidianInventoryItemInstance.h"

void UOInventoryItemFragment_Affixes::OnInstancedCreated(UObsidianInventoryItemInstance* Instance) const
{
	Instance->SetItemRarity(ItemRarityTag);

	//TODO(intrxx) #AffixRefactor
	// if(!bStartsIdentified && ItemAffixes.Num() > 0)
	// {
	// 	Instance->SetIdentified(false);
	// }
	// else if (bStartsIdentified || ItemRarityTag == ObsidianGameplayTags::Item_Rarity_Normal || ItemAffixes.IsEmpty())
	// {
	// 	Instance->SetIdentified(true);
	// }
	//
	// for(const FObsidianDynamicItemAffix& Affix : ItemAffixes)
	// {
	// 	Instance->AddAffix(Affix);
	// }
}

void UOInventoryItemFragment_Affixes::AddItemAffixes(const TArray<FObsidianDynamicItemAffix>& InItemAffixes, const FGameplayTag& InRarityTag)
{
	// Change name to initialize/create
	// Clear item affixes to make sure we are okay\
	// check against affix/prefix constrains
	
	ItemRarityTag = InRarityTag;
	
	DynamicItemAffixes.Append(InItemAffixes);
}

bool UOInventoryItemFragment_Affixes::IsItemIdentified() const
{
	//TODO(intrxx) #AffixRefactor
	// if(!bStartsIdentified && ItemAffixes.Num() > 0)
	// {
	// 	return false;
	// }
	return true;
}

EObsidianAffixGenerationType UOInventoryItemFragment_Affixes::GetGenerationType() const
{
	return ItemAffixesGenerationType;
}

TArray<FObsidianAffixDescriptionRow> UOInventoryItemFragment_Affixes::GetAffixesAsUIDescription() const
{
	TArray<FObsidianAffixDescriptionRow> AffixDescriptionRows;
	//AffixDescriptionRows.Reserve(ItemAffixes.Num());

	//TODO(intrxx) #AffixRefactor
	// for(const FObsidianDynamicItemAffix& Affix : ItemAffixes)
	// {
	// 	if(Affix)
	// 	{
	// 		FObsidianAffixDescriptionRow Row;
	// 		Row.AffixTag = Affix.AffixTag;
	// 		Row.SetAffixRowDescription(Affix.AffixDescription, Affix.TempAffixMagnitude);
	// 		Row.SetAffixAdditionalDescription(Affix.AffixType, Affix.AffixTier);
	// 		AffixDescriptionRows.Add(Row);
	// 	}
	// }
	return AffixDescriptionRows;
}
