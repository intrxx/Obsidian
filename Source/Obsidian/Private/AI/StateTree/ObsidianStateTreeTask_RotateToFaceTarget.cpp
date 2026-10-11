// Copyright 2026 out of sCope team - intrxx

#include "AI/StateTree/ObsidianStateTreeTask_RotateToFaceTarget.h"

#include "AIController.h"
#include "AITypes.h"
#include "StateTreeExecutionContext.h"


#define LOCTEXT_NAMESPACE "ObsidianStateTree"

EStateTreeRunStatus FObsidianStateTreeTask_RotateToFaceTarget::EnterState(FStateTreeExecutionContext& InContext, const FStateTreeTransitionResult& InTransition) const
{
	FInstanceDataType& InstanceData = InContext.GetInstanceData(*this);
	InstanceData.FocusActorSet = nullptr;

	AAIController* AIController = InstanceData.AIController;
	if(AIController == nullptr || AIController->GetPawn() == nullptr || InstanceData.Target == nullptr)
	{
		return EStateTreeRunStatus::Failed;
	}

	if(IsFacingLocation(*AIController, InstanceData.Target->GetActorLocation()))
	{
		return EStateTreeRunStatus::Succeeded;
	}

	AIController->SetFocus(InstanceData.Target, EAIFocusPriority::Gameplay);
	InstanceData.FocusActorSet = InstanceData.Target;
	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FObsidianStateTreeTask_RotateToFaceTarget::Tick(FStateTreeExecutionContext& InContext, const float InDeltaTime) const
{
	FInstanceDataType& InstanceData = InContext.GetInstanceData(*this);

	const AAIController* AIController = InstanceData.AIController;
	if(AIController == nullptr || AIController->GetPawn() == nullptr)
	{
		return EStateTreeRunStatus::Failed;
	}

	const FVector FocalPoint = AIController->GetFocalPointForPriority(EAIFocusPriority::Gameplay);
	if(FocalPoint == FAISystem::InvalidLocation)
	{
		CleanUp(InstanceData);
		return EStateTreeRunStatus::Failed;
	}

	if(IsFacingLocation(*AIController, FocalPoint))
	{
		CleanUp(InstanceData);
		return EStateTreeRunStatus::Succeeded;
	}

	return EStateTreeRunStatus::Running;
}

void FObsidianStateTreeTask_RotateToFaceTarget::ExitState(FStateTreeExecutionContext& InContext, const FStateTreeTransitionResult& InTransition) const
{
	CleanUp(InContext.GetInstanceData(*this));
}

bool FObsidianStateTreeTask_RotateToFaceTarget::IsFacingLocation(const AAIController& InAIController, const FVector& InLocation) const
{
	const APawn* Pawn = InAIController.GetPawn();
	const FVector PawnDirection = Pawn->GetActorForwardVector();
	const FVector ToLocation = InLocation - Pawn->GetActorLocation();

	// The same check the Rotate to face BB entry Task of the Behavior Tree does.
	const double AngleDifferenceDot = (PawnDirection.IsNearlyZero() || ToLocation.IsNearlyZero()) ? 1.0 : PawnDirection.CosineAngle2D(ToLocation);
	return AngleDifferenceDot >= FMath::Cos(FMath::DegreesToRadians(Precision));
}

void FObsidianStateTreeTask_RotateToFaceTarget::CleanUp(FInstanceDataType& InOutInstanceData) const
{
	AAIController* AIController = InOutInstanceData.AIController;
	if(AIController && InOutInstanceData.FocusActorSet && InOutInstanceData.FocusActorSet == AIController->GetFocusActorForPriority(EAIFocusPriority::Gameplay))
	{
		AIController->ClearFocus(EAIFocusPriority::Gameplay);
	}
	InOutInstanceData.FocusActorSet = nullptr;
}

#if WITH_EDITOR
FText FObsidianStateTreeTask_RotateToFaceTarget::GetDescription(const FGuid& InID, FStateTreeDataView InInstanceDataView,
	const IStateTreeBindingLookup& InBindingLookup, EStateTreeNodeFormatting InFormatting) const
{
	const FText TargetText = InBindingLookup.GetBindingSourceDisplayName(FPropertyBindingPath(InID, GET_MEMBER_NAME_CHECKED(FInstanceDataType, Target)), InFormatting);
	return FText::Format(LOCTEXT("RotateToFaceTargetDescription", "Rotate To Face {0}"), TargetText.IsEmpty() ? LOCTEXT("RotateToFaceNoTarget", "None") : TargetText);
}
#endif

#undef LOCTEXT_NAMESPACE
