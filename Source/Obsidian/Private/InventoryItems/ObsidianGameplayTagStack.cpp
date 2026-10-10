// Copyright 2026 out of sCope team - intrxx

#include "InventoryItems//ObsidianGameplayTagStack.h"

#include "UObject/Stack.h"


// ---- Start of FGameplayTagStack ----

FString FGameplayTagStack::GetDebugString() const
{
	return  FString::Printf(TEXT("Stack: [%s,%d]"), *Tag.ToString(), StackCount);
}

// ---- Start of FGameplayTagStackContainer ----

void FGameplayTagStackContainer::AddStack(FGameplayTag InToTag, int32 InStackCount)
{
	if(!InToTag.IsValid())
	{
		FFrame::KismetExecutionMessage(TEXT("Tag passed to AddStack is invalid."), ELogVerbosity::Error);
		return;
	}

	if(InStackCount > 0)
	{
		for(FGameplayTagStack& Stack : Stacks)
		{
			if(Stack.Tag == InToTag)
			{
				const int32 NewCount = Stack.StackCount + InStackCount;
				Stack.StackCount = NewCount;
				TagToCountMap[InToTag] = NewCount;
				MarkItemDirty(Stack);
				return;
			}
		}

		FGameplayTagStack& NewStack = Stacks.Emplace_GetRef(InToTag, InStackCount);
		MarkItemDirty(NewStack);
		TagToCountMap.Add(InToTag, InStackCount);
		return;
	}
	FFrame::KismetExecutionMessage(TEXT("Trying to Add 0 or negative number of stacks, in case of the second one use RemoveStack."), ELogVerbosity::Error);
}

void FGameplayTagStackContainer::RemoveStack(FGameplayTag InFromTag, int32 InStackCount)
{
	if(!InFromTag.IsValid())
	{
		FFrame::KismetExecutionMessage(TEXT("Tag passed to RemoveStack is invalid."), ELogVerbosity::Error);
		return;
	}
	
	if(InStackCount > 0)
	{
		for(auto It = Stacks.CreateIterator(); It; ++It)
		{
			FGameplayTagStack& Stack = *It;
			if(Stack.Tag == InFromTag)
			{
				if(Stack.StackCount <= InStackCount)
				{
					if(Stack.StackCount != InStackCount)
					{
						FFrame::KismetExecutionMessage(TEXT("Passed StackCount to remove is greater than this item's StackCount."), ELogVerbosity::Warning);
					}
					It.RemoveCurrent();
					TagToCountMap.Remove(InFromTag);
					MarkArrayDirty();
				}
				else
				{
					const int32 NewCount = Stack.StackCount - InStackCount;
					Stack.StackCount = NewCount;
					TagToCountMap[InFromTag] = NewCount;
					MarkItemDirty(Stack);
				}
				return;
			}
		}
		FFrame::KismetExecutionMessage(TEXT("There is no Stack for provided Tag."), ELogVerbosity::Error);
		return;
	}
	FFrame::KismetExecutionMessage(TEXT("Trying to Remove 0 or negative number of stacks."), ELogVerbosity::Error);
}

void FGameplayTagStackContainer::OverrideStack(FGameplayTag InTag, int32 InNewStackCount)
{
	if(!InTag.IsValid())
	{
		FFrame::KismetExecutionMessage(TEXT("Tag passed to OverrideStack is invalid."), ELogVerbosity::Error);
		return;
	}

	if(InNewStackCount > 0)
	{
		for(FGameplayTagStack& Stack : Stacks)
		{
			if(Stack.Tag == InTag)
			{
				const int32 NewCount = InNewStackCount;
				Stack.StackCount = NewCount;
				TagToCountMap[InTag] = NewCount;
				MarkItemDirty(Stack);
				return;
			}
		}

		FGameplayTagStack& NewStack = Stacks.Emplace_GetRef(InTag, InNewStackCount);
		MarkItemDirty(NewStack);
		TagToCountMap.Add(InTag, InNewStackCount);
		return;
	}
	FFrame::KismetExecutionMessage(TEXT("Trying to Override 0 or negative number of stacks, in case of the second one use RemoveStack."), ELogVerbosity::Error);
}

void FGameplayTagStackContainer::PreReplicatedRemove(const TArrayView<int32> InRemovedIndices, int32 InFinalSize)
{
	for(const int32 Index : InRemovedIndices)
	{
		const FGameplayTag Tag = Stacks[Index].Tag;
		TagToCountMap.Remove(Tag);
	}
}

void FGameplayTagStackContainer::PostReplicatedAdd(const TArrayView<int32> InAddedIndices, int32 InFinalSize)
{
	for(const int32 Index : InAddedIndices)
	{
		const FGameplayTagStack& Stack = Stacks[Index];
		TagToCountMap.Add(Stack.Tag, Stack.StackCount);
	}
}

void FGameplayTagStackContainer::PostReplicatedChange(const TArrayView<int32> InChangedIndices, int32 InFinalSize)
{
	for(const int32 Index : InChangedIndices)
	{
		const FGameplayTagStack& Stack = Stacks[Index];
		TagToCountMap[Stack.Tag] = Stack.StackCount;
	}
}


