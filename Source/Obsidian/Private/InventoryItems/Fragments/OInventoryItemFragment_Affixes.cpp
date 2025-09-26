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
		TArray<FObsidianActiveItemAffix> AffixesToInitialize;
		if (StaticItemImplicit)
		{
			FObsidianActiveItemAffix ActiveItemAffix;
			ActiveItemAffix.InitializeWithStatic(StaticItemImplicit);
			AffixesToInitialize.Add(ActiveItemAffix);
		}

		for (const FObsidianStaticItemAffix& StaticItemAffix : StaticItemAffixes)
		{
			//TODO(intrxx) Validate?
			FObsidianActiveItemAffix ActiveItemAffix;
			ActiveItemAffix.InitializeWithStatic(StaticItemAffix);
			AffixesToInitialize.Add(ActiveItemAffix);
		}
			
		Instance->InitializeAffixes(AffixesToInitialize);
	}
	else if (ItemAffixesGenerationType == EObsidianAffixGenerationType::DefaultGeneration || ItemAffixesGenerationType == EObsidianAffixGenerationType::FullGeneration)
	{
		Instance->InitializeAffixes(InitializedDynamicAffixes);
	}
}

void UOInventoryItemFragment_Affixes::InitializeDynamicAffixes(const TArray<FObsidianDynamicItemAffix>& InItemAffixes, const FGameplayTag& InRarityTag)
{
	StaticItemAffixes.Empty();
	ItemRarityTag = InRarityTag;
	
	if (ItemAffixesGenerationType == EObsidianAffixGenerationType::DefaultGeneration && StaticItemImplicit)
	{
		FObsidianActiveItemAffix ActiveItemAffix;
		ActiveItemAffix.InitializeWithStatic(StaticItemImplicit);
		InitializedDynamicAffixes.Add(ActiveItemAffix);
	}
	
	InitializedDynamicAffixes.Empty(InItemAffixes.Num());
	for (const FObsidianDynamicItemAffix& DynamicItemAffix : InItemAffixes)
	{
		//TODO(intrxx) Validate?
		FObsidianActiveItemAffix ActiveItemAffix;
		ActiveItemAffix.InitializeWithDynamic(DynamicItemAffix);
		InitializedDynamicAffixes.Add(ActiveItemAffix);
	}
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
	
	if (InitializedDynamicAffixes.IsEmpty() == false)
	{
		AffixDescriptionRows.Reserve(InitializedDynamicAffixes.Num());
	}
	else if (StaticItemAffixes.IsEmpty() == false)
	{
		AffixDescriptionRows.Reserve(StaticItemAffixes.Num());
	}
	
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
