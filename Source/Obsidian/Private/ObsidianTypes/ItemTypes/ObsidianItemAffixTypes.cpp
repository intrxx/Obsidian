// Copyright 2024 out of sCope team - Michał Ogiński

#include "ObsidianTypes/ItemTypes/ObsidianItemAffixTypes.h"

// ~ Core

// ~ Project
#include "InventoryItems/ObsidianItemAffixStack.h"

// ~ FObsidianStaticItemAffix

bool FObsidianStaticItemAffix::IsEmptyImplicit() const
{
	return AffixType == EObsidianAffixType::Implicit && IsEmptyAffix();
}

bool FObsidianStaticItemAffix::IsEmptyAffix() const
{
	return PossibleAffixRanges.IsEmpty();
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
	AffixValueType = InDynamicItemAffix.AffixValueType;
	PossibleAffixRanges = InDynamicItemAffix.PossibleAffixRanges;
	SoftGameplayEffectToApply = InDynamicItemAffix.SoftGameplayEffectToApply;

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
	AffixValueType = InStaticItemAffix.AffixValueType;
	PossibleAffixRanges = InStaticItemAffix.PossibleAffixRanges;
	SoftGameplayEffectToApply = InStaticItemAffix.SoftGameplayEffectToApply;

	InitializeAffixTierAndRange(UpToTreasureQuality);
}

void FObsidianActiveItemAffix::InitializeAffixTierAndRange(const uint8 UpToTreasureQuality)
{
	//TODO(intrxx) Get Random Range in weighted way bound by Treasure Quality
	FObsidianAffixValueRange ChosenAffixValueTier = GetRandomAffixRange(UpToTreasureQuality);
	for (const FFloatRange& AffixRange : ChosenAffixValueTier.AffixRanges)
	{
		float RandomisedValue = FMath::FRandRange(AffixRange.GetLowerBoundValue(), AffixRange.GetUpperBoundValue());
		RandomisedValue = AffixValueType == EObsidianAffixValueType::Int ? FMath::FloorToInt(RandomisedValue) : RandomisedValue;
		CurrentAffixValue.CurrentAffixValues.Add(RandomisedValue);
	}
	CurrentAffixValue.AffixTier = ChosenAffixValueTier.AffixTier.AffixTierValue;
	CreateAffixActiveDescription();
}

void FObsidianActiveItemAffix::RandomizeAffixValueBoundByRange()
{
	CurrentAffixValue.CurrentAffixValues.Empty();

	TArray<FFloatRange> CurrentPossibleFloatRanges;
	for (const FObsidianAffixValueRange& AffixValueRange : PossibleAffixRanges)
	{
		if (AffixValueRange.AffixTier.AffixTierValue == CurrentAffixValue.AffixTier)
		{
			CurrentPossibleFloatRanges = AffixValueRange.AffixRanges;
		}
	}
	check(CurrentPossibleFloatRanges.IsEmpty() == false);
	
	for (const FFloatRange& AffixRange : CurrentPossibleFloatRanges)
	{
		float RandomisedValue = FMath::FRandRange(AffixRange.GetLowerBoundValue(), AffixRange.GetUpperBoundValue());
		RandomisedValue = AffixValueType == EObsidianAffixValueType::Int ? FMath::FloorToInt(RandomisedValue) : RandomisedValue;
		CurrentAffixValue.CurrentAffixValues.Add(RandomisedValue);
	}
	
	CreateAffixActiveDescription();
}

FObsidianAffixValueRange FObsidianActiveItemAffix::GetRandomAffixRange(const uint8 UpToTreasureQuality)
{
	uint32 TotalWeight = 0;
	TArray<FObsidianAffixValueRange> CanRollFromAffixRanges;
	for (const FObsidianAffixValueRange& Value : PossibleAffixRanges)
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
	Args.Append(CurrentAffixValue.CurrentAffixValues);

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
