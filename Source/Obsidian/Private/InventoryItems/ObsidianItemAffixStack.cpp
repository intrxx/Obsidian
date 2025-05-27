// Copyright 2024 out of sCope team - Michał Ogiński

#include "InventoryItems/ObsidianItemAffixStack.h"

// ~ Core

// ~ Project

int32 FObsidianItemAffixStack::GetTotalAffixCount() const
{
	return Entries.Num();
}

int32 FObsidianItemAffixStack::GetPrefixCount() const
{
	int32 PrefixCount = 0;
	for(const FObsidianAffixEntry& Entry : Entries)
	{
		if(Entry.ItemAffix.AffixType == EObsidianAffixType::Prefix)
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
		if(Entry.ItemAffix.AffixType == EObsidianAffixType::Suffix)
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
		if(Entry.ItemAffix.AffixType == EObsidianAffixType::Implicit)
		{
			return true;
		}
	}
	return false;
}

TArray<FObsidianItemAffix> FObsidianItemAffixStack::GetAllItemAffixes() const
{
	TArray<FObsidianItemAffix> Affixes;
	Affixes.Reserve(Entries.Num());

	for(const FObsidianAffixEntry& Entry : Entries)
	{
		if(Entry.ItemAffix)
		{
			Affixes.Add(Entry.ItemAffix);
		}
	}
	return Affixes;
}

void FObsidianItemAffixStack::AddAffix(const FObsidianItemAffix& ItemAffix)
{
	FObsidianAffixEntry& AffixEntry = Entries.Add_GetRef(ItemAffix);

	MarkItemDirty(AffixEntry);
}

void FObsidianItemAffixStack::RemoveAffix(const FGameplayTag& AffixTag)
{
	for(auto It = Entries.CreateIterator(); It; ++It)
	{
		FObsidianAffixEntry& Entry = *It;
		if(Entry.ItemAffix.AffixTag == AffixTag)
		{
			It.RemoveCurrent();
			MarkArrayDirty();
		}
	}
}

void FObsidianItemAffixStack::AffixChanged(const FGameplayTag& AffixTag)
{
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
