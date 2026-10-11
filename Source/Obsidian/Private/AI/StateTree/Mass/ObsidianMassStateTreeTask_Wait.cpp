// Copyright 2026 out of sCope team - intrxx

#include "AI/StateTree/Mass/ObsidianMassStateTreeTask_Wait.h"

#include "MassCommonFragments.h"
#include "MassMovementFragments.h"
#include "MassNavigationFragments.h"
#include "MassNavMeshNavigationFragments.h"
#include "MassNavMeshNavigationUtils.h"
#include "MassSignalSubsystem.h"
#include "MassStateTreeExecutionContext.h"
#include "StateTreeLinker.h"


namespace ObsidianMassWait
{
	/** Signal can come a moment before the whole time is counted down, anything closer than this is treated as done. */
	constexpr float TimeTolerance = 0.02f;
}

bool FObsidianMassStateTreeTask_Wait::Link(FStateTreeLinker& InLinker)
{
	InLinker.LinkExternalData(SignalSubsystemHandle);
	InLinker.LinkExternalData(MoveTargetHandle);
	InLinker.LinkExternalData(ShortPathHandle);
	InLinker.LinkExternalData(MovementParamsHandle);
	InLinker.LinkExternalData(TransformHandle);
	return true;
}

EStateTreeRunStatus FObsidianMassStateTreeTask_Wait::EnterState(FStateTreeExecutionContext& InContext, const FStateTreeTransitionResult& InTransition) const
{
	const FMassStateTreeExecutionContext& MassContext = static_cast<FMassStateTreeExecutionContext&>(InContext);
	FInstanceDataType& InstanceData = InContext.GetInstanceData(*this);

	if(bStopMovement)
	{
		const FMassMovementParameters& MovementParams = InContext.GetExternalData(MovementParamsHandle);
		FMassNavMeshShortPathFragment& ShortPath = InContext.GetExternalData(ShortPathHandle);
		FMassMoveTargetFragment& MoveTarget = InContext.GetExternalData(MoveTargetHandle);

		MoveTarget.Center = InContext.GetExternalData(TransformHandle).GetTransform().GetLocation();
		MoveTarget.CreateNewAction(EMassMovementAction::Stand, *InContext.GetWorld());
		if(UE::MassNavigation::ActivateActionStand(InContext.GetOwner(), MassContext.GetEntity(), MovementParams.DefaultDesiredSpeed, MoveTarget, ShortPath) == false)
		{
			return EStateTreeRunStatus::Failed;
		}
	}

	InstanceData.TimeLeft = 0.0f;
	if(InstanceData.Duration > 0.0f)
	{
		InstanceData.TimeLeft = FMath::FRandRange(FMath::Max(0.0f, InstanceData.Duration - InstanceData.RandomDeviation), InstanceData.Duration + InstanceData.RandomDeviation);

		UMassSignalSubsystem& SignalSubsystem = InContext.GetExternalData(SignalSubsystemHandle);
		SignalSubsystem.DelaySignalEntityDeferred(MassContext.GetMassEntityExecutionContext(), UE::Mass::Signals::StandTaskFinished,
			MassContext.GetEntity(), InstanceData.TimeLeft);
	}

	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FObsidianMassStateTreeTask_Wait::Tick(FStateTreeExecutionContext& InContext, const float InDeltaTime) const
{
	FInstanceDataType& InstanceData = InContext.GetInstanceData(*this);
	if(InstanceData.Duration <= 0.0f)
	{
		return EStateTreeRunStatus::Running;
	}

	InstanceData.TimeLeft -= InDeltaTime;
	return InstanceData.TimeLeft <= ObsidianMassWait::TimeTolerance ? EStateTreeRunStatus::Succeeded : EStateTreeRunStatus::Running;
}
