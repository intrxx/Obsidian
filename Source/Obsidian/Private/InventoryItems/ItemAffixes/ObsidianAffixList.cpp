// Copyright 2024 out of sCope team - Michał Ogiński

#include "InventoryItems/ItemAffixes/ObsidianAffixList.h"

// ~ Core
#include "UObject/ObjectSaveContext.h"
#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif // ~ With Editor

// ~ Project

// ~ FObsidianAffixClass

TArray<FObsidianDynamicItemAffix> FObsidianAffixClass::GetAllAffixesUpToQuality(const int32 UpToTreasureQuality) const
{
	TArray<FObsidianDynamicItemAffix> MatchingTreasureClasses;
	
	for (const FObsidianDynamicItemAffix& Class : ItemAffixList)
	{
		if (Class.MinItemLevelRequirement <= UpToTreasureQuality)
		{
			MatchingTreasureClasses.Add(Class);
		}
	}

	return MatchingTreasureClasses;
}

TArray<FObsidianDynamicItemAffix> FObsidianAffixClass::GetAllAffixesUpToQualityForCategory(const int32 UpToTreasureQuality, const FGameplayTag& ForCategory) const
{
	TArray<FObsidianDynamicItemAffix> MatchingTreasureClasses;
	
	for (const FObsidianDynamicItemAffix& Class : ItemAffixList)
	{
		if (Class.MinItemLevelRequirement <= UpToTreasureQuality && Class.AcceptedItemCategories.HasTagExact(ForCategory))
		{
			MatchingTreasureClasses.Add(Class);
		}
	}

	return MatchingTreasureClasses;
}

// ~ FObsidianAffixClass

UObsidianAffixList::UObsidianAffixList(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UObsidianAffixList::PostInitProperties()
{
	Super::PostInitProperties();

	for (FObsidianAffixClass& AffixClass : AffixClasses)
	{
		AffixClass.ItemAffixList.Sort([](const FObsidianDynamicItemAffix& A, const FObsidianDynamicItemAffix& B)
				{
					return A.MinItemLevelRequirement > B.MinItemLevelRequirement;
				});
	}
}

void UObsidianAffixList::PostLoad()
{
	Super::PostLoad();
}

TArray<FObsidianAffixClass> UObsidianAffixList::GetAllAffixClasses() const
{
	return AffixClasses;
}

void UObsidianAffixList::PreSave(FObjectPreSaveContext SaveContext)
{
	Super::PreSave(SaveContext);
	
	for (FObsidianAffixClass& Class : AffixClasses)
	{
		const EObsidianAffixType AffixType = Class.AffixClassType;
		for (FObsidianDynamicItemAffix& Affix : Class.ItemAffixList)
		{
			Affix.AffixType = AffixType;
		}
	}
}

void UObsidianAffixList::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	for (FObsidianAffixClass& Class : AffixClasses)
	{
		const EObsidianAffixType AffixType = Class.AffixClassType;
		for (FObsidianDynamicItemAffix& Affix : Class.ItemAffixList)
		{
			Affix.AffixType = AffixType;
		}
	}
}

#if WITH_EDITOR
EDataValidationResult FObsidianAffixClass::ValidateData(FDataValidationContext& Context, const FName ClassName, const int Index) const
{
	EDataValidationResult Result = EDataValidationResult::Valid;
	
	const FGameplayTag AffixTag = FGameplayTag::RequestGameplayTag(TEXT("Item.Affix"));
	for (int32 i = 0; i < ItemAffixList.Num(); i++)
	{
		const FObsidianDynamicItemAffix DynamicAffix = ItemAffixList[i];
		
		if (DynamicAffix.AffixTag.IsValid() == false || DynamicAffix.AffixTag.MatchesTag(AffixTag) == false)
		{
			Result = EDataValidationResult::Invalid;
			
			const FText ErrorMessage = FText::FromString(FString::Printf(TEXT("Affix Tag at index [%i] of [%s] class at index [%i] is invalid! \n"
				"Please fill correct Affix Tag."), i, *ClassName.ToString(), Index));
			Context.AddError(ErrorMessage);
		}
		
		if (DynamicAffix.AcceptedItemCategories.IsEmpty())
		{
			Result = EDataValidationResult::Invalid;

			const FText ErrorMessage = FText::FromString(FString::Printf(TEXT("Accepted Item Categories at index [%i] of [%s] class at index [%i] are empty! \n"
				"Please fill Accepted Item Categories container with Item Category tags."), i, *ClassName.ToString(), Index));
			Context.AddError(ErrorMessage);
		}

		// Most Affixes get the default AffixAbilitySets from ItemDataDeveloperSettings
		// if (DynamicAffix.SoftAbilitySetToApply.IsNull())
		// {
		// 	Result = EDataValidationResult::Invalid;
		//
		// 	const FText ErrorMessage = FText::FromString(FString::Printf(TEXT("SoftAbilitySetToApply at index [%i] of [%s] class at index [%i] is not set! \n"
		// 		"Please provide a valid AbilitySet to apply!"), i, *ClassName.ToString(), Index));
		// 	Context.AddError(ErrorMessage);
		// }

		if (DynamicAffix.AffixValuesDefinition.IsValid() == false)
		{
			Result = EDataValidationResult::Invalid;

			const FText ErrorMessage = FText::FromString(FString::Printf(TEXT("PossibleAffixRanges at index [%i] of [%s] class at index [%i] are not set! \n"
				"Please fill it with possible affix ranges."), i, *ClassName.ToString(), Index));
			Context.AddError(ErrorMessage);
			continue;
		}
		
		uint8 ExpectedCount = DynamicAffix.AffixValuesDefinition.AffixValuesIdentifiers.Num();
		for (int32 y = 0; y < DynamicAffix.AffixValuesDefinition.PossibleAffixRanges.Num(); y++)
		{
			const FObsidianAffixValueRange Range = DynamicAffix.AffixValuesDefinition.PossibleAffixRanges[y];
			if (Range.AffixRanges.Num() != ExpectedCount)
			{
				Result = EDataValidationResult::Invalid;

				const FText ErrorMessage = FText::FromString(FString::Printf(TEXT("Number of AffixRanges at index [%i] inside [%i] Affix of [%s] class at index [%i] differs from expected number of [%d]! \n"
					"Please make sure that every entry has the same number of possible ranges."),y, i, *ClassName.ToString(), Index, ExpectedCount));
				Context.AddError(ErrorMessage);
			}
		}
	}

	return Result;
}

EDataValidationResult UObsidianAffixList::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(Context), EDataValidationResult::Valid);

	uint16 TreasureClassesIndex = 0;
	for (const FObsidianAffixClass& Class : AffixClasses)
	{
		Result =  CombineDataValidationResults(Result, Class.ValidateData(Context, Class.AffixClassName, TreasureClassesIndex));
		TreasureClassesIndex++;
	}
	
	return Result;
}
#endif


