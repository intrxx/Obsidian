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
}

bool UOInventoryItemFragment_Affixes::HasImplicitAffix() const
{
	return bHasImplicitAffix && StaticItemImplicit;
}

FObsidianStaticItemAffix UOInventoryItemFragment_Affixes::GetStaticImplicitAffix() const
{
	return StaticItemImplicit;
}

TArray<FObsidianStaticItemAffix> UOInventoryItemFragment_Affixes::GetStaticAffixes() const
{
	return StaticItemAffixes;
}

EObsidianAffixGenerationType UOInventoryItemFragment_Affixes::GetGenerationType() const
{
	return ItemAffixesGenerationType;
}
