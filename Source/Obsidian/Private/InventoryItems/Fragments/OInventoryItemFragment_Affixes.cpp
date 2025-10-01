// Copyright 2024 out of sCope team - Michał Ogiński

#include "InventoryItems/Fragments/OInventoryItemFragment_Affixes.h"

// ~ Core

// ~ Project
#include "InventoryItems/ObsidianInventoryItemInstance.h"

UOInventoryItemFragment_Affixes::UOInventoryItemFragment_Affixes(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	StaticItemImplicit.AffixType = EObsidianAffixType::Implicit;
}

void UOInventoryItemFragment_Affixes::OnInstancedCreated(UObsidianInventoryItemInstance* Instance) const
{
	Instance->SetItemRarity(ItemRarityTag);
	Instance->SetIdentified(IsItemIdentified());
	
	if (ItemAffixesGenerationType == EObsidianAffixGenerationType::NoGeneration)
	{
		TArray<FObsidianStaticItemAffix> AffixesToInitialize;
		AffixesToInitialize.Reserve(StaticItemAffixes.Num());
		AffixesToInitialize.Append(StaticItemAffixes);
		
		if (StaticItemImplicit)
		{
			AffixesToInitialize.Add(StaticItemImplicit);
		}
		
		Instance->InitializeAffixes(AffixesToInitialize);
	}
	else if (ItemAffixesGenerationType == EObsidianAffixGenerationType::DefaultGeneration || ItemAffixesGenerationType == EObsidianAffixGenerationType::FullGeneration)
	{
		Instance->InitializeAffixes(AddedDynamicItemAffixes, StaticItemImplicit);
	}
}

void UOInventoryItemFragment_Affixes::InitializeDynamicAffixes(const TArray<FObsidianDynamicItemAffix>& InItemAffixes, const FGameplayTag& InRarityTag)
{
	StaticItemAffixes.Empty();
	ItemRarityTag = InRarityTag;
	AddedDynamicItemAffixes.Reserve(InItemAffixes.Num());
	AddedDynamicItemAffixes.Append(InItemAffixes);
}

void UOInventoryItemFragment_Affixes::RandomiseStaticAffixValues()
{
	if (ItemAffixesGenerationType == EObsidianAffixGenerationType::FullGeneration)
	{
		return;
	}

	if (ItemAffixesGenerationType == EObsidianAffixGenerationType::DefaultGeneration && StaticItemImplicit)
	{
		StaticItemImplicit.RandomizeRanges();
		return;
	}

	if (ItemAffixesGenerationType == EObsidianAffixGenerationType::NoGeneration)
	{
		StaticItemImplicit.RandomizeRanges();

		for (FObsidianStaticItemAffix& StaticAffix : StaticItemAffixes)
		{
			StaticAffix.RandomizeRanges();
		}
	}
}

bool UOInventoryItemFragment_Affixes::IsItemIdentified() const
{
	return bStartsIdentified || ItemRarityTag == ObsidianGameplayTags::Item_Rarity_Normal;
}

EObsidianAffixGenerationType UOInventoryItemFragment_Affixes::GetGenerationType() const
{
	return ItemAffixesGenerationType;
}

TArray<FObsidianAffixDescriptionRow> UOInventoryItemFragment_Affixes::GetAffixesAsUIDescription() const
{
	TArray<FObsidianAffixDescriptionRow> AffixDescriptionRows;
	const int32 NumberOfAffixes = AddedDynamicItemAffixes.Num() + StaticItemAffixes.Num() + (StaticItemImplicit ? 1 : 0);
	AffixDescriptionRows.Reserve(NumberOfAffixes);

	if (StaticItemImplicit)
	{
		FObsidianAffixDescriptionRow Row;
		Row.AffixTag = StaticItemImplicit.AffixTag;
		//TODO(intrxx) #AffixRefactor
		// Row.SetAffixRowDescription(StaticItemImplicit.AffixDescription, Affix.TempAffixMagnitude);
		// Row.SetAffixAdditionalDescription(StaticItemImplicit.AffixType, Affix.AffixTier);
		AffixDescriptionRows.Add(Row);
	}
	
	
	for(const FObsidianDynamicItemAffix& DynamicAffix : AddedDynamicItemAffixes)
	{
		if(DynamicAffix)
		{
			FObsidianAffixDescriptionRow Row;
			Row.AffixTag = DynamicAffix.AffixTag;
			//TODO(intrxx) #AffixRefactor
			// Row.SetAffixRowDescription(DynamicAffix.AffixDescription, DynamicAffix.TempAffixMagnitude);
			// Row.SetAffixAdditionalDescription(DynamicAffix.AffixType, DynamicAffix.AffixTier);
			AffixDescriptionRows.Add(Row);
		}
	}

	for(const FObsidianStaticItemAffix& StaticAffix : StaticItemAffixes)
	{
		if(StaticAffix)
		{
			FObsidianAffixDescriptionRow Row;
			Row.AffixTag = StaticAffix.AffixTag;
			//TODO(intrxx) #AffixRefactor
			// Row.SetAffixRowDescription(StaticAffix.AffixDescription, StaticAffix.TempAffixMagnitude);
			// Row.SetAffixAdditionalDescription(StaticAffix.AffixType, StaticAffix.AffixTier);
			AffixDescriptionRows.Add(Row);
		}
	}
	
	return AffixDescriptionRows;
}
