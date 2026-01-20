// Copyright 2026 out of sCope team - intrxx

#include "InventoryItems/ItemAffixes/ObsidianItemAffixStack.h"

// ~ Core
#include "GameplayEffect.h"

// ~ Project
#include "InventoryItems/ItemAffixes/ObsidianAffixList.h"

DEFINE_LOG_CATEGORY(LogAffixes);

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

