// Copyright 2026 out of sCope team - intrxx

#include "AI/StateTree/ObsidianStateTreePropertyFunction_MakeGameplayTagContainer.h"

#include "StateTreeExecutionContext.h"


#define LOCTEXT_NAMESPACE "ObsidianStateTree"

void FObsidianStateTreePropertyFunction_MakeGameplayTagContainer::Execute(FStateTreeExecutionContext& InContext) const
{
	FInstanceDataType& InstanceData = InContext.GetInstanceData(*this);

	// Instance data persists between executions, so rebuild the container from scratch every time.
	InstanceData.Result.Reset(InstanceData.Tags.Num());
	for(const FGameplayTag& Tag : InstanceData.Tags)
	{
		// AddTag skips invalid tags and duplicates.
		InstanceData.Result.AddTag(Tag);
	}
}

#if WITH_EDITOR
FText FObsidianStateTreePropertyFunction_MakeGameplayTagContainer::GetDescription(const FGuid& InID, FStateTreeDataView InInstanceDataView,
	const IStateTreeBindingLookup& InBindingLookup, EStateTreeNodeFormatting InFormatting) const
{
	return LOCTEXT("MakeGameplayTagContainerDescription", "Make Gameplay Tag Container");
}
#endif

#undef LOCTEXT_NAMESPACE
