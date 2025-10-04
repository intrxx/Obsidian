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

void FObsidianActiveItemAffix::InitializeWithDynamic(const FObsidianDynamicItemAffix& InDynamicItemAffix)
{
	if (!InDynamicItemAffix)
	{
		UE_LOG(LogAffixes, Warning, TEXT("Initializing Affix failed, InDynamicItemAffix is invalid."))
		return;
	}

	AffixTag = InDynamicItemAffix.AffixTag;
	AffixDescription = InDynamicItemAffix.AffixDescription;
	AffixItemNameAddition = InDynamicItemAffix.AffixItemNameAddition;
	AffixType = InDynamicItemAffix.AffixType;
	AffixValueType = InDynamicItemAffix.AffixValueType;
	PossibleAffixRanges = InDynamicItemAffix.PossibleAffixRanges;
	SoftGameplayEffectToApply = InDynamicItemAffix.SoftGameplayEffectToApply;
}

void FObsidianActiveItemAffix::InitializeWithStatic(const FObsidianStaticItemAffix& InStaticItemAffix)
{
	if (!InStaticItemAffix)
	{
		UE_LOG(LogAffixes, Warning, TEXT("Initializing Affix failed, InStaticItemAffix is invalid."))
		return;
	}

	AffixTag = InStaticItemAffix.AffixTag;
	AffixDescription = InStaticItemAffix.AffixDescription;
	AffixItemNameAddition = InStaticItemAffix.AffixItemNameAddition;
	AffixType = InStaticItemAffix.AffixType;
	AffixValueType = InStaticItemAffix.AffixValueType;
	PossibleAffixRanges = { FObsidianAffixValue(InStaticItemAffix.PossibleAffixRanges) };
	SoftGameplayEffectToApply = InStaticItemAffix.SoftGameplayEffectToApply;
}

void FObsidianActiveItemAffix::InitializeAffixTierAndRange()
{
	//TODO(intrxx) Get Random Range in weighted way
	FObsidianAffixValue ChosenAffixValueTier = PossibleAffixRanges[FMath::RandRange(0, PossibleAffixRanges.Num() - 1)];
	for (const FFloatRange& AffixRange : ChosenAffixValueTier.AffixRanges)
	{
		float RandomisedValue = FMath::FRandRange(AffixRange.GetLowerBoundValue(), AffixRange.GetUpperBoundValue());
		RandomisedValue = AffixValueType == EObsidianAffixValueType::Int ? FMath::FloorToInt(RandomisedValue) : RandomisedValue;
		ChosenAffixValueTier.CurrentAffixValues.Add(RandomisedValue);
	}
	CurrentAffixValue = ChosenAffixValueTier;
}

void FObsidianActiveItemAffix::RandomizeAffixValue()
{
	CurrentAffixValue.CurrentAffixValues.Empty();
	for (const FFloatRange& AffixRange : CurrentAffixValue.AffixRanges)
	{
		float RandomisedValue = FMath::FRandRange(AffixRange.GetLowerBoundValue(), AffixRange.GetUpperBoundValue());
		RandomisedValue = AffixValueType == EObsidianAffixValueType::Int ? FMath::FloorToInt(RandomisedValue) : RandomisedValue;
		CurrentAffixValue.CurrentAffixValues.Add(RandomisedValue);
	}
}

// ~ FObsidianDescriptionAffixRow

void FObsidianAffixDescriptionRow::SetAffixRowDescription(const FText& InAffixDescription, const int32 InTempMagnitude)
{
	AffixRowDescription = FText::FromString(FString::Printf(TEXT("Adds %d %s"), InTempMagnitude, *InAffixDescription.ToString()));
}

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