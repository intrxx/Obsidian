// Copyright 2024 out of sCope team - Michał Ogiński

#include "InventoryItems/Fragments/OInventoryItemFragment_Affixes.h"

#include <UObject/ObjectSaveContext.h>

#include "InventoryItems/ObsidianInventoryItemInstance.h"

UOInventoryItemFragment_Affixes::UOInventoryItemFragment_Affixes(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	StaticItemImplicit.AffixType = EObsidianAffixType::Implicit;
	StaticItemSkillImplicit.AffixType = EObsidianAffixType::SkillImplicit;
}

void UOInventoryItemFragment_Affixes::OnInstancedCreated(UObsidianInventoryItemInstance* Instance) const
{
}

void UOInventoryItemFragment_Affixes::PostInitProperties()
{
	Super::PostInitProperties();

	for (FObsidianStaticItemAffix& PrimaryAffix : PrimaryItemAffixes)
	{
		PrimaryAffix.AffixType = EObsidianAffixType::PrimaryItemAffix;
	}
}

void UOInventoryItemFragment_Affixes::PreSave(FObjectPreSaveContext SaveContext)
{
	Super::PreSave(SaveContext);

	for (FObsidianStaticItemAffix& PrimaryAffix : PrimaryItemAffixes)
	{
		PrimaryAffix.AffixType = EObsidianAffixType::PrimaryItemAffix;
	}
}

bool UOInventoryItemFragment_Affixes::HasPrimaryItemAffix() const
{
	return bHasPrimaryItemAffix && PrimaryItemAffixes.IsEmpty() == false;
}

bool UOInventoryItemFragment_Affixes::HasImplicitAffix() const
{
	return bHasImplicitAffix && StaticItemImplicit;
}

TArray<FObsidianStaticItemAffix> UOInventoryItemFragment_Affixes::GetPrimaryItemAffixes() const
{
	return PrimaryItemAffixes;
}

FObsidianStaticItemAffix UOInventoryItemFragment_Affixes::GetStaticImplicitAffix() const
{
	return StaticItemImplicit;
}

FObsidianStaticItemAffix UOInventoryItemFragment_Affixes::GetStaticSkillImplicitAffix() const
{
	return StaticItemSkillImplicit;
}

TArray<FObsidianStaticItemAffix> UOInventoryItemFragment_Affixes::GetStaticAffixes() const
{
	return StaticItemAffixes;
}

EObsidianAffixGenerationType UOInventoryItemFragment_Affixes::GetGenerationType() const
{
	return ItemAffixesGenerationType;
}
