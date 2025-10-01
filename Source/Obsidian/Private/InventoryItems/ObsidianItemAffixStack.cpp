// Copyright 2024 out of sCope team - Michał Ogiński

#include "InventoryItems/ObsidianItemAffixStack.h"

// ~ Core
#include "GameplayEffect.h"

// ~ Project

DEFINE_LOG_CATEGORY(LogAffixes);

// ~ FObsidianAffixEntry

void FObsidianAffixEntry::InitializeWithDynamic(const FObsidianDynamicItemAffix& InDynamicItemAffix)
{
	if (!InDynamicItemAffix)
	{
		UE_LOG(LogAffixes, Warning, TEXT("Initializing Affix failed, InDynamicItemAffix is invalid."))
		return;
	}

	ActiveItemAffix.AffixTag = InDynamicItemAffix.AffixTag;
	ActiveItemAffix.AffixDescription = InDynamicItemAffix.AffixDescription;
	ActiveItemAffix.AffixItemNameAddition = InDynamicItemAffix.AffixItemNameAddition;
	ActiveItemAffix.AffixType = InDynamicItemAffix.AffixType;
	ActiveItemAffix.AffixValueType = InDynamicItemAffix.AffixValueType;
	ActiveItemAffix.PossibleAffixRanges = InDynamicItemAffix.PossibleAffixRanges;
	ActiveItemAffix.RandomisedRanges = InDynamicItemAffix.RandomisedRanges;
	
	if (const TSubclassOf<UGameplayEffect>& AffixGameplayEffectRef = InDynamicItemAffix.SoftGameplayEffectToApply.Get())
	{
		ActiveItemAffix.GameplayEffectToApply = AffixGameplayEffectRef;
	}
	else
	{
		UE_LOG(LogAffixes, Warning, TEXT("Soft Gameplay Effect Class of Affix [%s] is not loaded, switching to LoadSynchronous()."),
			*InDynamicItemAffix.AffixTag.GetTagName().ToString());
		ActiveItemAffix.GameplayEffectToApply = InDynamicItemAffix.SoftGameplayEffectToApply.LoadSynchronous();
	}
}

void FObsidianAffixEntry::InitializeWithStatic(const FObsidianStaticItemAffix& InStaticItemAffix)
{
	if (!InStaticItemAffix)
	{
		UE_LOG(LogAffixes, Warning, TEXT("Initializing Affix failed, InStaticItemAffix is invalid."))
		return;
	}

	ActiveItemAffix.AffixTag = InStaticItemAffix.AffixTag;
	ActiveItemAffix.AffixDescription = InStaticItemAffix.AffixDescription;
	ActiveItemAffix.AffixItemNameAddition = InStaticItemAffix.AffixItemNameAddition;
	ActiveItemAffix.AffixType = InStaticItemAffix.AffixType;
	ActiveItemAffix.AffixValueType = InStaticItemAffix.AffixValueType;
	ActiveItemAffix.PossibleAffixRanges = { FObsidianAffixRange(InStaticItemAffix.PossibleAffixRanges) };
	ActiveItemAffix.RandomisedRanges = InStaticItemAffix.RandomisedRanges;
	
	if (const TSubclassOf<UGameplayEffect>& AffixGameplayEffectRef = InStaticItemAffix.SoftGameplayEffectToApply.Get())
	{
		ActiveItemAffix.GameplayEffectToApply = AffixGameplayEffectRef;
	}
	else
	{
		UE_LOG(LogAffixes, Warning, TEXT("Soft Gameplay Effect Class of Affix [%s] is not loaded, switching to LoadSynchronous()."),
			*InStaticItemAffix.AffixTag.GetTagName().ToString());
		ActiveItemAffix.GameplayEffectToApply = InStaticItemAffix.SoftGameplayEffectToApply.LoadSynchronous();
	}
}

// ~ End of FObsidianAffixEntry

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

void FObsidianItemAffixStack::InitializeAffixes(UObsidianInventoryItemInstance* InOwningInstance, const TArray<FObsidianStaticItemAffix>& StaticAffixesToInitialize, const FObsidianStaticItemAffix& StaticImplicit)
{
	check(Entries.IsEmpty());
	for (const FObsidianStaticItemAffix& StaticAffix : StaticAffixesToInitialize)
	{
		if (StaticAffix)
		{
			FObsidianAffixEntry& AffixEntry = Entries.Add_GetRef(InOwningInstance);
			AffixEntry.InitializeWithStatic(StaticAffix);
			//TODO(intrxx) Apply Affix Gameplay Effect with correct Magnitude/Magnitudes
			MarkItemDirty(AffixEntry);
		}
	}
	InitializeImplicit(InOwningInstance, StaticImplicit);
}

void FObsidianItemAffixStack::InitializeAffixes(UObsidianInventoryItemInstance* InOwningInstance, const TArray<FObsidianDynamicItemAffix>& DynamicAffixesToInitialize, const FObsidianStaticItemAffix& StaticImplicit)
{
	check(Entries.IsEmpty());
	for (const FObsidianDynamicItemAffix& DynamicAffix : DynamicAffixesToInitialize)
	{
		if (DynamicAffix)
		{
			FObsidianAffixEntry& AffixEntry = Entries.Add_GetRef(InOwningInstance);
			AffixEntry.InitializeWithDynamic(DynamicAffix);
			//TODO(intrxx) Apply Affix Gameplay Effect with correct Magnitude/Magnitudes
			MarkItemDirty(AffixEntry);
		}
	}
	InitializeImplicit(InOwningInstance, StaticImplicit);
}

void FObsidianItemAffixStack::InitializeImplicit(UObsidianInventoryItemInstance* InOwningInstance, const FObsidianStaticItemAffix& StaticImplicit)
{
	check(HasImplicit() == false)
	if (StaticImplicit)
	{
		FObsidianAffixEntry& ImplicitAffixEntry = Entries.Add_GetRef(InOwningInstance);
		ImplicitAffixEntry.InitializeWithStatic(StaticImplicit);
		//TODO(intrxx) Apply Affix Gameplay Effect with correct Magnitude/Magnitudes
		MarkItemDirty(ImplicitAffixEntry);
	}
}

void FObsidianItemAffixStack::AddAffix(UObsidianInventoryItemInstance* InOwningInstance, const FObsidianDynamicItemAffix& ItemAffix)
{
	FObsidianAffixEntry& AffixEntry = Entries.Add_GetRef(InOwningInstance);
	AffixEntry.InitializeWithDynamic(ItemAffix);
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

