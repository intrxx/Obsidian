// Copyright 2024 out of sCope team - Michał Ogiński

#include "InventoryItems/ObsidianItemAffixStack.h"

// ~ Core
#include "GameplayEffect.h"

// ~ Project
#include "InventoryItems/ItemAffixes/ObsidianAffixList.h"

DEFINE_LOG_CATEGORY(LogAffixes);

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

// ~ End of FObsidianActiveItemAffix

int32 FObsidianItemAffixStack::GetTotalAffixCount() const
{
	return Entries.Num();
}

int32 FObsidianItemAffixStack::GetPrefixCount() const
{
	int32 PrefixCount = 0;
	for(const FObsidianAffixEntry& Entry : Entries)
	{
		if(Entry.ActiveItemAffix.AffixType == EObsidianAffixType::Prefix)
		{
			PrefixCount++;
		}
	} 
	return PrefixCount;
}

int32 FObsidianItemAffixStack::GetSuffixCount() const
{
	int32 SuffixCount = 0;
	for(const FObsidianAffixEntry& Entry : Entries)
	{
		if(Entry.ActiveItemAffix.AffixType == EObsidianAffixType::Suffix)
		{
			SuffixCount++;
		}
	}
	return SuffixCount;
}

bool FObsidianItemAffixStack::HasImplicit() const
{
	 for(const FObsidianAffixEntry& Entry : Entries)
	 {
	 	if(Entry.ActiveItemAffix.AffixType == EObsidianAffixType::Implicit)
	 	{
	 		return true;
	 	}
	 }
	return false;
}

TArray<FObsidianActiveItemAffix> FObsidianItemAffixStack::GetAllItemAffixes() const
{
	TArray<FObsidianActiveItemAffix> Affixes;
	Affixes.Reserve(Entries.Num());
	for(const FObsidianAffixEntry& Entry : Entries)
	{
		if(Entry.ActiveItemAffix)
		{
			Affixes.Add(Entry.ActiveItemAffix);
		}
	}
	return Affixes;
}

void FObsidianItemAffixStack::InitializeAffixes(UObsidianInventoryItemInstance* InOwningInstance, const TArray<FObsidianActiveItemAffix>& AffixesToInitialize)
{
	check(Entries.IsEmpty());
	for (const FObsidianActiveItemAffix& Affix : AffixesToInitialize)
	{
		if (Affix)
		{
			FObsidianAffixEntry& AffixEntry = Entries.Add_GetRef(Affix);
			AffixEntry.OwningItem = InOwningInstance;
			
			//TODO(intrxx) Apply Affix Gameplay Effect with correct Magnitude/Magnitudes
			
			MarkItemDirty(AffixEntry);
		}
	}
}

void FObsidianItemAffixStack::AddAffix(UObsidianInventoryItemInstance* InOwningInstance, const FObsidianActiveItemAffix& ItemAffix)
{
	FObsidianAffixEntry& AffixEntry = Entries.Add_GetRef(ItemAffix);
	AffixEntry.OwningItem = InOwningInstance;
	
	//TODO(intrxx) Apply Affix Gameplay Effect with correct Magnitude/Magnitudes
	
	MarkItemDirty(AffixEntry);
}

void FObsidianItemAffixStack::RemoveAffix(const FGameplayTag& AffixTag)
{
	for(auto It = Entries.CreateIterator(); It; ++It)
	{
		FObsidianAffixEntry& Entry = *It;
		if(Entry.ActiveItemAffix.AffixTag == AffixTag)
		{
			It.RemoveCurrent();
			
			//TODO(intrxx) Remove Affix Gameplay Effect with correct Magnitude/Magnitudes
			
			MarkArrayDirty();
		}
	}
}

void FObsidianItemAffixStack::AffixChanged(const FGameplayTag& AffixTag)
{
	//TODO(intrxx) Reapply Affix Gameplay Effect with correct new Magnitude/Magnitudes?
}

void FObsidianItemAffixStack::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
}

void FObsidianItemAffixStack::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
}

void FObsidianItemAffixStack::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
}

