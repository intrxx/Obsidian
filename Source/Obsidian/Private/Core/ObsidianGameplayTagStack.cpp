// Copyright 2024 out of sCope team - Michał Ogiński

#include "Core/ObsidianGameplayTagStack.h"
#include "UObject/Stack.h"

// ---- Start of FGameplayTagStack ----

FString FGameplayTagStack::GetDebugString() const
{
	return  FString::Printf(TEXT("Stack: [%s,%d]"), *Tag.ToString(), StackCount);
}

// ---- Start of FGameplayTagStackContainer ----

void FGameplayTagStackContainer::AddStack(FGameplayTag ToTag, int32 StackCount)
{
	if(!ToTag.IsValid())
	{
		FFrame::KismetExecutionMessage(TEXT("Tag passed to AddStack is invalid."), ELogVerbosity::Error);
		return;
	}

	if(StackCount > 0)
	{
		for(FGameplayTagStack& Stack : Stacks)
		{
			if(Stack.Tag == ToTag)
			{
				const int32 NewCount = Stack.StackCount + StackCount;
				Stack.StackCount = NewCount;
				TagToCountMap[ToTag] = NewCount;
				MarkItemDirty(Stack);
				return;
			}
		}

		FGameplayTagStack& NewStack = Stacks.Emplace_GetRef(ToTag, StackCount);
		MarkItemDirty(NewStack);
		TagToCountMap.Add(ToTag, StackCount);
		return;
	}
	FFrame::KismetExecutionMessage(TEXT("Trying to Add 0 or negative number of stacks, in case of the second one use RemoveStack."), ELogVerbosity::Error);
}

void FGameplayTagStackContainer::RemoveStack(FGameplayTag FromTag, int32 StackCount)
{
	if(!FromTag.IsValid())
	{
		FFrame::KismetExecutionMessage(TEXT("Tag passed to RemoveStack is invalid."), ELogVerbosity::Error);
		return;
	}

	bool bRemoveSuccess = false;
	
	if(StackCount > 0)
	{
		for(auto It = Stacks.CreateIterator(); It; ++It)
		{
			FGameplayTagStack& Stack = *It;
			if(Stack.Tag == FromTag)
			{
				if(Stack.StackCount <= StackCount)
				{
					if(Stack.StackCount != StackCount)
					{
						FFrame::KismetExecutionMessage(TEXT("Passed StackCount to remove is greater than this item's StackCount."), ELogVerbosity::Warning);
					}
					It.RemoveCurrent();
					TagToCountMap.Remove(FromTag);
					MarkArrayDirty();
					bRemoveSuccess = true;
				}
				else
				{
					const int32 NewCount = Stack.StackCount - StackCount;
					Stack.StackCount = NewCount;
					TagToCountMap[FromTag] = NewCount;
					MarkItemDirty(Stack);
					bRemoveSuccess = true;
				}
				return;
			}
		}
	}
	
	if(bRemoveSuccess == false)
	{
		FFrame::KismetExecutionMessage(TEXT("There is no Stack for provided Tag."), ELogVerbosity::Error);
		return;
	}
	FFrame::KismetExecutionMessage(TEXT("Trying to Remove 0 or negative number of stacks."), ELogVerbosity::Error);
}

void FGameplayTagStackContainer::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	for(const int32 Index : RemovedIndices)
	{
		const FGameplayTag Tag = Stacks[Index].Tag;
		TagToCountMap.Remove(Tag);
	}
}

void FGameplayTagStackContainer::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for(const int32 Index : AddedIndices)
	{
		const FGameplayTagStack& Stack = Stacks[Index];
		TagToCountMap.Add(Stack.Tag, Stack.StackCount);
	}
}

void FGameplayTagStackContainer::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	for(const int32 Index : ChangedIndices)
	{
		const FGameplayTagStack& Stack = Stacks[Index];
		TagToCountMap[Stack.Tag] = Stack.StackCount;
	}
}


