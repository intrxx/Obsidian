// Copyright 2024 out of sCope team - Michał Ogiński

#include "InventoryItems/Fragments/OInventoryItemFragment_Affixes.h"

#include "InventoryItems/ObsidianInventoryItemInstance.h"

void UOInventoryItemFragment_Affixes::OnInstancedCreated(UObsidianInventoryItemInstance* Instance) const
{
	Instance->SetItemRarity(ItemRarityTag);
	Instance->SetIdentified(bIdentified);
	
	for(const FObsidianItemAffix& Affix : ItemAffixes)
	{
		Instance->AddAffix(Affix);
	}
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
