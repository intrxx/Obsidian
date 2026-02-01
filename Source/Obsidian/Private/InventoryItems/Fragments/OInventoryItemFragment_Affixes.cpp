// Copyright 2026 out of sCope team - intrxx

#include "InventoryItems/Fragments/OInventoryItemFragment_Affixes.h"

#include <UObject/ObjectSaveContext.h>
#if WITH_EDITOR
#include <Misc/DataValidation.h>
#endif

#include "InventoryItems/ObsidianInventoryItemInstance.h"

UOInventoryItemFragment_Affixes::UOInventoryItemFragment_Affixes(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	StaticItemImplicit.AffixType = EObsidianAffixType::Implicit;
	StaticItemSkillImplicit.AffixType = EObsidianAffixType::SkillImplicit;
}

void UOInventoryItemFragment_Affixes::OnInstancedCreated(UObsidianInventoryItemInstance* Instance) const
{
	if (Instance)
	{
		Instance->SetCanHaveAffixes(true);
	}
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

#if WITH_EDITOR
	FDataValidationContext Context;
	IsDataValid(Context);

	for (const FDataValidationContext::FIssue& Issue : Context.GetIssues())
	{
		if (Issue.Severity == EMessageSeverity::Error)
		{
			UE_LOG(LogTemp, Error, TEXT("Issue: %s"), *Issue.Message.ToString());
		}
		else if (Issue.Severity == EMessageSeverity::Warning)
		{
			UE_LOG(LogTemp, Warning, TEXT("Issue: %s"), *Issue.Message.ToString());
		}
	}
#endif
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

#if WITH_EDITOR
EDataValidationResult UOInventoryItemFragment_Affixes::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(Context), EDataValidationResult::Valid);

	if (bHasPrimaryItemAffix)
	{
		for (int32 i = 0; i < PrimaryItemAffixes.Num(); ++i)
		{
			Result = CombineDataValidationResults(Result, PrimaryItemAffixes[i].IsStaticAffixValid(Context, i, TEXT("Primary Item")));
		}
	}

	if (bHasImplicitAffix)
	{
		Result = CombineDataValidationResults(Result, StaticItemImplicit.IsStaticAffixValid(Context, 0, TEXT("Implicit")));
	}

	if (ItemAffixesGenerationType == EObsidianAffixGenerationType::NoGeneration)
	{
		Result = CombineDataValidationResults(Result, StaticItemSkillImplicit.IsStaticAffixValid(Context, 0, TEXT("Skill Implicit")));

		for (int32 i = 0; i < StaticItemAffixes.Num(); ++i)
		{
			Result = CombineDataValidationResults(Result, StaticItemAffixes[i].IsStaticAffixValid(Context, 0, TEXT("Static Item ")));
		}
	}
	
	return Result;
}
#endif