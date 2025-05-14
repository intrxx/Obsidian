// Copyright 2024 out of sCope team - Michał Ogiński

#include "InventoryItems/Fragments/OInventoryItemFragment_Affixes.h"
#include "InventoryItems/ObsidianInventoryItemInstance.h"

void UOInventoryItemFragment_Affixes::OnInstancedCreated(UObsidianInventoryItemInstance* Instance) const
{
	Instance->SetItemRarity(ItemRarityTag);
	
	if(!bStartsIdentified && ItemAffixes.Num() > 0)
	{
		Instance->SetIdentified(false);
	}
	else if (bStartsIdentified || ItemRarityTag == ObsidianGameplayTags::Item_Rarity_Normal || ItemAffixes.IsEmpty())
	{
		Instance->SetIdentified(true);
	}
	
	for(const FObsidianItemAffix& Affix : ItemAffixes)
	{
		Instance->AddAffix(Affix);
	}
}

bool UOInventoryItemFragment_Affixes::IsItemIdentified() const
{
	if(!bStartsIdentified && ItemAffixes.Num() > 0)
	{
		return false;
	}
	return true;
}

TArray<FObsidianAffixDescriptionRow> UOInventoryItemFragment_Affixes::GetAffixesAsUIDescription() const
{
	TArray<FObsidianAffixDescriptionRow> AffixDescriptionRows;
	AffixDescriptionRows.Reserve(ItemAffixes.Num());

	for(FObsidianItemAffix Affix : ItemAffixes)
	{
		if(Affix)
		{
			FObsidianAffixDescriptionRow Row;
			Row.AffixTag = Affix.AffixTag;
			Row.SetAffixRowDescription(Affix.AffixDescription, Affix.TempAffixMagnitude);
			Row.SetAffixAdditionalDescription(Affix.AffixType, Affix.AffixTier);
			AffixDescriptionRows.Add(Row);
		}
	}
	return AffixDescriptionRows;
}
