// Copyright 2026 out of sCope team - intrxx

#include "AI/StateTree/ObsidianStateTreeTask_SetFocus.h"

#include "AIController.h"
#include "StateTreeExecutionContext.h"

#include "Obsidian/ObsidianLogCategories.h"


#define LOCTEXT_NAMESPACE "ObsidianStateTree"

FObsidianStateTreeTask_SetFocus::FObsidianStateTreeTask_SetFocus()
{
	bShouldCallTick = false;
	// The Focus Target is read once, when the State is entered.
	bShouldCopyBoundPropertiesOnTick = false;
	bShouldCopyBoundPropertiesOnExitState = false;

#if WITH_EDITORONLY_DATA
	bConsideredForCompletion = false;
	bCanEditConsideredForCompletion = false;
#endif
}

EStateTreeRunStatus FObsidianStateTreeTask_SetFocus::EnterState(FStateTreeExecutionContext& InContext, const FStateTreeTransitionResult& InTransition) const
{
	const FInstanceDataType& InstanceData = InContext.GetInstanceData(*this);

	if(InstanceData.AIController == nullptr)
	{
		UE_LOG(ObLogAI, Error, TEXT("FObsidianStateTreeTask_SetFocus failed since the AI Controller is missing."));
		return EStateTreeRunStatus::Failed;
	}

	if(InstanceData.FocusTarget == nullptr)
	{
		UE_LOG(ObLogAI, Error, TEXT("FObsidianStateTreeTask_SetFocus failed since the Target to Focus is missing."));
		return EStateTreeRunStatus::Failed;
	}

	InstanceData.AIController->SetFocus(InstanceData.FocusTarget);
	return EStateTreeRunStatus::Running;
}

void FObsidianStateTreeTask_SetFocus::ExitState(FStateTreeExecutionContext& InContext, const FStateTreeTransitionResult& InTransition) const
{
	const FInstanceDataType& InstanceData = InContext.GetInstanceData(*this);

	if(InstanceData.AIController)
	{
		InstanceData.AIController->ClearFocus(EAIFocusPriority::Gameplay);
	}
}

#if WITH_EDITOR
FText FObsidianStateTreeTask_SetFocus::GetDescription(const FGuid& InID, FStateTreeDataView InInstanceDataView,
	const IStateTreeBindingLookup& InBindingLookup, EStateTreeNodeFormatting InFormatting) const
{
	const FText TargetText = InBindingLookup.GetBindingSourceDisplayName(FPropertyBindingPath(InID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, FocusTarget)), InFormatting);
	return FText::Format(LOCTEXT("SetFocusDescription", "Set Focus on {0}"), TargetText.IsEmpty() ? LOCTEXT("SetFocusNoTarget", "None") : TargetText);
}
#endif

#undef LOCTEXT_NAMESPACE
