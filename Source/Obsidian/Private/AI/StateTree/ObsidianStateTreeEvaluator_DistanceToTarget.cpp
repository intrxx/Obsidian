// Copyright 2026 out of sCope team - intrxx

#include "AI/StateTree/ObsidianStateTreeEvaluator_DistanceToTarget.h"

#include "GameFramework/Actor.h"
#include "StateTreeExecutionContext.h"


void FObsidianStateTreeEvaluator_DistanceToTarget::TreeStart(FStateTreeExecutionContext& InContext) const
{
	UpdateDistance(InContext.GetInstanceData(*this));
}

void FObsidianStateTreeEvaluator_DistanceToTarget::Tick(FStateTreeExecutionContext& InContext, const float InDeltaTime) const
{
	FInstanceDataType& InstanceData = InContext.GetInstanceData(*this);

	InstanceData.TimeToNextUpdate -= InDeltaTime;
	if(InstanceData.TimeToNextUpdate <= 0.0f)
	{
		UpdateDistance(InstanceData);
	}
}

void FObsidianStateTreeEvaluator_DistanceToTarget::UpdateDistance(FInstanceDataType& InOutInstanceData) const
{
	InOutInstanceData.TimeToNextUpdate = FMath::FRandRange(FMath::Max(0.0f, Interval - RandomDeviation), Interval + RandomDeviation);

	// The same as it was on the Behavior Tree Service, lack of the Target results in the distance of 0.
	InOutInstanceData.DistanceToTarget = InOutInstanceData.Actor ? InOutInstanceData.Actor->GetDistanceTo(InOutInstanceData.TargetActor) : 0.0f;
}
