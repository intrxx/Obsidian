// Copyright 2024 out of sCope team - Michał Ogiński

#include "ObsidianTypes/ItemTypes/ObsidianItemAffixTypes.h"


#include "InventoryItems/ItemAffixes/ObsidianItemAffixStack.h"


bool FObsidianAffixValues::IsValid() const
{
	return AffixValuesIdentifiers.IsEmpty() == false && PossibleAffixRanges.IsEmpty() == false;
}

// ~ FObsidianStaticItemAffix

bool FObsidianStaticItemAffix::IsEmptyImplicit() const
{
	return AffixType == EObsidianAffixType::Implicit && IsEmptyAffix();
}

bool FObsidianStaticItemAffix::IsEmptyAffix() const
{
	return AffixValuesDefinition.IsValid();
}

FObsidianStaticItemAffix::operator bool() const
{
	return AffixTag.IsValid();
}

bool FObsidianStaticItemAffix::operator==(const FObsidianStaticItemAffix& Other) const
{
	return AffixTag == Other.AffixTag;
}

bool FObsidianStaticItemAffix::operator==(const FObsidianDynamicItemAffix& Other) const
{
	return AffixTag == Other.AffixTag;
}

bool FObsidianStaticItemAffix::operator==(const FObsidianActiveItemAffix& Other) const
{
	return AffixTag == Other.AffixTag;
}

// ~ FObsidianDynamicItemAffix

FObsidianDynamicItemAffix::operator bool() const
{
	return AffixTag.IsValid();
}

bool FObsidianDynamicItemAffix::operator==(const FObsidianDynamicItemAffix& Other) const
{
	return AffixTag == Other.AffixTag;
}

bool FObsidianDynamicItemAffix::operator==(const FObsidianActiveItemAffix& Other) const
{
	return AffixTag == Other.AffixTag;
}

bool FObsidianDynamicItemAffix::operator==(const FObsidianStaticItemAffix& Other) const
{
	return AffixTag == Other.AffixTag;
}

// ~ FObsidianActiveItemAffix

bool FObsidianActiveItemAffix::operator==(const FObsidianActiveItemAffix& Other) const
{
	return AffixTag == Other.AffixTag;
}

bool FObsidianActiveItemAffix::operator==(const FObsidianDynamicItemAffix& Other) const
{
	return AffixTag == Other.AffixTag;
}

bool FObsidianActiveItemAffix::operator==(const FObsidianStaticItemAffix& Other) const
{
	return AffixTag == Other.AffixTag;
}

uint8 FObsidianActiveItemAffix::GetCurrentAffixTier() const
{
	return CurrentAffixValue.AffixTier;
}

void FObsidianActiveItemAffix::InitializeWithDynamic(const FObsidianDynamicItemAffix& InDynamicItemAffix, const uint8 UpToTreasureQuality)
{
	if (!InDynamicItemAffix)
	{
		UE_LOG(LogAffixes, Warning, TEXT("Initializing Affix failed, InDynamicItemAffix is invalid."))
		return;
	}

	AffixTag = InDynamicItemAffix.AffixTag;
	UnformattedAffixDescription = InDynamicItemAffix.AffixDescription;
	AffixItemNameAddition = InDynamicItemAffix.AffixItemNameAddition;
	AffixType = InDynamicItemAffix.AffixType;
	AffixValuesDefinition = InDynamicItemAffix.AffixValuesDefinition;
	SoftAbilitySetToApply = InDynamicItemAffix.SoftAbilitySetToApply;

	InitializeAffixTierAndRange(UpToTreasureQuality);
}

void FObsidianActiveItemAffix::InitializeWithStatic(const FObsidianStaticItemAffix& InStaticItemAffix, const uint8 UpToTreasureQuality)
{
	if (!InStaticItemAffix)
	{
		UE_LOG(LogAffixes, Warning, TEXT("Initializing Affix failed, InStaticItemAffix is invalid."))
		return;
	}

	AffixTag = InStaticItemAffix.AffixTag;
	UnformattedAffixDescription = InStaticItemAffix.AffixDescription;
	AffixItemNameAddition = InStaticItemAffix.AffixItemNameAddition;
	AffixType = InStaticItemAffix.AffixType;
	AffixValuesDefinition = InStaticItemAffix.AffixValuesDefinition;
	SoftAbilitySetToApply = InStaticItemAffix.SoftAbilitySetToApply;

	InitializeAffixTierAndRange(UpToTreasureQuality);
}

void FObsidianActiveItemAffix::InitializeAffixTierAndRange(const uint8 UpToTreasureQuality)
{
	FObsidianAffixValueRange ChosenAffixValueTier = GetRandomAffixRange(UpToTreasureQuality);
	for (int32 i = 0; i < ChosenAffixValueTier.AffixRanges.Num(); ++i)
	{
		FFloatRange& AffixRange = ChosenAffixValueTier.AffixRanges[i];
		float RandomisedValue = FMath::FRandRange(AffixRange.GetLowerBoundValue(), AffixRange.GetUpperBoundValue());
		RandomisedValue = AffixValuesDefinition.AffixValueType == EObsidianAffixValueType::Int ?
				FMath::FloorToInt(RandomisedValue) : FMath::RoundToFloat(RandomisedValue * 100.0f) / 100.0f;
		
		CurrentAffixValue.AffixValuesIdentifiers.Add(AffixValuesDefinition.AffixValuesIdentifiers[i]);	
		CurrentAffixValue.AffixValues.Add(RandomisedValue);	
	}
	CurrentAffixValue.AffixTier = ChosenAffixValueTier.AffixTier.AffixTierValue;
	CreateAffixActiveDescription();
}

void FObsidianActiveItemAffix::RandomizeAffixValueBoundByRange()
{
	TArray<FFloatRange> CurrentPossibleFloatRanges;
	for (const FObsidianAffixValueRange& AffixValueRange : AffixValuesDefinition.PossibleAffixRanges)
	{
		if (AffixValueRange.AffixTier.AffixTierValue == CurrentAffixValue.AffixTier)
		{
			CurrentPossibleFloatRanges = AffixValueRange.AffixRanges;
		}
	}
	check(CurrentPossibleFloatRanges.IsEmpty() == false);
	
	for (int32 i = 0; i < CurrentPossibleFloatRanges.Num(); ++i)
	{
		FFloatRange& AffixRange = CurrentPossibleFloatRanges[i];
		float RandomisedValue = FMath::FRandRange(AffixRange.GetLowerBoundValue(), AffixRange.GetUpperBoundValue());
		RandomisedValue = AffixValuesDefinition.AffixValueType == EObsidianAffixValueType::Int ?
				FMath::FloorToInt(RandomisedValue) : FMath::RoundToFloat(RandomisedValue * 100.0f) / 100.0f;
		
		CurrentAffixValue.AffixValuesIdentifiers.Add(AffixValuesDefinition.AffixValuesIdentifiers[i]);	
		CurrentAffixValue.AffixValues.Add(RandomisedValue);	
	}
	
	CreateAffixActiveDescription();
}

FObsidianAffixValueRange FObsidianActiveItemAffix::GetRandomAffixRange(const uint8 UpToTreasureQuality)
{
	uint32 TotalWeight = 0;
	TArray<FObsidianAffixValueRange> CanRollFromAffixRanges;
	for (const FObsidianAffixValueRange& Value : AffixValuesDefinition.PossibleAffixRanges)
	{
		if (Value.AffixTier.MinItemLevelRequirement <= UpToTreasureQuality)
		{
			TotalWeight += Value.AffixTierWeight;
			CanRollFromAffixRanges.Add(Value);
		}
	}

	const uint32 Roll = FMath::RandRange(0, TotalWeight);
	uint32 Cumulative = 0;
	
	for (const FObsidianAffixValueRange& Value : CanRollFromAffixRanges)
	{
		Cumulative += Value.AffixTierWeight;
		if (Roll <= Cumulative)
		{
			return Value;
		}
	}

	checkf(false, TEXT("No Affix Range was returned from GetRandomAffixRange."))
	return FObsidianAffixValueRange();
}

void FObsidianActiveItemAffix::CreateAffixActiveDescription()
{
	TArray<FFormatArgumentValue> Args;
	for (const float AffixValue : CurrentAffixValue.AffixValues)
	{
		Args.Add(AffixValue);
	}
	ActiveAffixDescription = FText::Format(UnformattedAffixDescription, Args);
}

// ~ FObsidianRareItemNameGenerationData

FText FObsidianRareItemNameGenerationData::GetRandomPrefixNameAddition(const int32 UpToTreasureQuality)
{
	TArray<FText> PrefixAdditionsCandidates;
	for (const FObsidianRareItemNameAddition& PrefixAdditions : PrefixNameAdditions)
	{
		if (PrefixAdditions.ItemLevelRange.X > UpToTreasureQuality)
		{
			continue;
		}

		PrefixAdditionsCandidates.Append(PrefixAdditions.ItemNameAdditions);
	}

	check(PrefixAdditionsCandidates.IsEmpty() == false);
	const int32 RandomInt = FMath::RandRange(0, PrefixAdditionsCandidates.Num() - 1);
	return PrefixAdditionsCandidates[RandomInt];
}

FText FObsidianRareItemNameGenerationData::GetRandomSuffixNameAddition(const int32 UpToTreasureQuality, const FGameplayTag& ForItemCategory)
{
	TArray<FText> PrefixAdditionsCandidates;
	for (const FObsidianRareItemSuffixNameAddition& SuffixAdditionClass : SuffixNameAdditions)
	{
		if (SuffixAdditionClass.ForItemCategories.HasTagExact(ForItemCategory) == false)
		{
			continue;
		}
		
		for (const FObsidianRareItemNameAddition& SuffixAdditions : SuffixAdditionClass.ItemNameAdditions)
		{
			if (SuffixAdditions.ItemLevelRange.X > UpToTreasureQuality)
			{
				continue;
			}

			PrefixAdditionsCandidates.Append(SuffixAdditions.ItemNameAdditions);
		}
	}
	
	check(PrefixAdditionsCandidates.IsEmpty() == false);
	const int32 RandomInt = FMath::RandRange(0, PrefixAdditionsCandidates.Num() - 1);
	return PrefixAdditionsCandidates[RandomInt];
}

// ~ FObsidianDescriptionAffixRow

void FObsidianAffixDescriptionRow::SetAffixAdditionalDescription(const EObsidianAffixType& InAffixType, const int32 InAffixTier)
{
	AffixType = InAffixType;
	FText AffixTypeText = FText();
	
	switch (InAffixType)
	{
	case EObsidianAffixType::Implicit:
		AffixTypeText = FText::FromString(FString::Printf(TEXT("Implicit, ")));
		break;
	case EObsidianAffixType::Prefix:
		AffixTypeText = FText::FromString(FString::Printf(TEXT("Prefix, ")));
		break;
	case EObsidianAffixType::Suffix:
		AffixTypeText = FText::FromString(FString::Printf(TEXT("Suffix, ")));
		break;
	default:
		AffixTypeText = FText::FromString(FString::Printf(TEXT("Affix, ")));
		break;
	}
	
	AffixAdditionalDescription = FText::FromString(FString::Printf(TEXT("%s tier: %d "), *AffixTypeText.ToString(), InAffixTier));
}

// ~ End of FObsidianDescriptionAffixRow
