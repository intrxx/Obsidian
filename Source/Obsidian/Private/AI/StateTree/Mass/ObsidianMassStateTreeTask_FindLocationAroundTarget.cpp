// Copyright 2026 out of sCope team - intrxx

#include "AI/StateTree/Mass/ObsidianMassStateTreeTask_FindLocationAroundTarget.h"

#include "NavigationSystem.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeLinker.h"

#include "Mass/ObsidianMassEnemyFragments.h"


FObsidianMassStateTreeTask_FindLocationAroundTarget::FObsidianMassStateTreeTask_FindLocationAroundTarget()
{
	bShouldCallTick = false;

#if WITH_EDITORONLY_DATA
	bConsideredForCompletion = false;
	bCanEditConsideredForCompletion = false;
#endif
}

bool FObsidianMassStateTreeTask_FindLocationAroundTarget::Link(FStateTreeLinker& InLinker)
{
	InLinker.LinkExternalData(CombatTargetHandle);
	return true;
}

EStateTreeRunStatus FObsidianMassStateTreeTask_FindLocationAroundTarget::EnterState(FStateTreeExecutionContext& InContext, const FStateTreeTransitionResult& InTransition) const
{
	const FObsidianMassCombatTargetFragment& CombatTarget = InContext.GetExternalData(CombatTargetHandle);
	FInstanceDataType& InstanceData = InContext.GetInstanceData(*this);
	InstanceData.LocationAroundTarget.Reset();

	if(CombatTarget.HasTarget() == false)
	{
		return EStateTreeRunStatus::Failed;
	}

	const FVector Origin = CombatTarget.TargetLocation;
	FNavLocation RandomPoint(Origin);
	const UNavigationSystemV1* NavigationSystem = Cast<UNavigationSystemV1>(InContext.GetWorld()->GetNavigationSystem());
	const bool bFound = NavigationSystem && NavigationSystem->GetRandomPointInNavigableRadius(Origin, RadiusAroundTheTarget, RandomPoint);
	if(bFound == false && bFallBackToTargetIfFailed == false)
	{
		return EStateTreeRunStatus::Failed;
	}

	InstanceData.LocationAroundTarget.EndOfPathPosition = bFound ? RandomPoint.Location : Origin;
	return EStateTreeRunStatus::Running;
}
