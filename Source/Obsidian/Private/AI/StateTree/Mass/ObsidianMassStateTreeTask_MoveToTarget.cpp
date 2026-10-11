// Copyright 2026 out of sCope team - intrxx

#include "AI/StateTree/Mass/ObsidianMassStateTreeTask_MoveToTarget.h"

#include "StateTreeExecutionContext.h"


EStateTreeRunStatus FObsidianMassStateTreeTask_MoveToTarget::EnterState(FStateTreeExecutionContext& InContext, const FStateTreeTransitionResult& InTransition) const
{
	const EStateTreeRunStatus Status = Super::EnterState(InContext, InTransition);

	FInstanceDataType& InstanceData = InContext.GetInstanceData<FInstanceDataType>(*this);
	InstanceData.PathGoal = InstanceData.TargetLocation.EndOfPathPosition.Get(FVector::ZeroVector);
	return Status;
}

EStateTreeRunStatus FObsidianMassStateTreeTask_MoveToTarget::Tick(FStateTreeExecutionContext& InContext, const float InDeltaTime) const
{
	FInstanceDataType& InstanceData = InContext.GetInstanceData<FInstanceDataType>(*this);

	if(InstanceData.TargetLocation.EndOfPathPosition.IsSet())
	{
		const FVector NewGoal = InstanceData.TargetLocation.EndOfPathPosition.GetValue();
		if(FVector::DistSquared(NewGoal, InstanceData.PathGoal) > FMath::Square(InstanceData.RepathDistance))
		{
			if(RequestPath(InContext, InstanceData.TargetLocation) == false)
			{
				return EStateTreeRunStatus::Failed;
			}
			InstanceData.PathGoal = NewGoal;
			return EStateTreeRunStatus::Running;
		}
	}

	return Super::Tick(InContext, InDeltaTime);
}
