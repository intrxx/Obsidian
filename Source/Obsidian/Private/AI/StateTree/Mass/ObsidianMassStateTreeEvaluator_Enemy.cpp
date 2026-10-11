// Copyright 2026 out of sCope team - intrxx

#include "AI/StateTree/Mass/ObsidianMassStateTreeEvaluator_Enemy.h"

#include "StateTreeExecutionContext.h"
#include "StateTreeLinker.h"

#include "Mass/ObsidianMassEnemyFragments.h"


bool FObsidianMassStateTreeEvaluator_Enemy::Link(FStateTreeLinker& InLinker)
{
	InLinker.LinkExternalData(CombatTargetHandle);
	InLinker.LinkExternalData(EnemyStateHandle);
	return true;
}

void FObsidianMassStateTreeEvaluator_Enemy::TreeStart(FStateTreeExecutionContext& InContext) const
{
	Update(InContext);
}

void FObsidianMassStateTreeEvaluator_Enemy::Tick(FStateTreeExecutionContext& InContext, const float InDeltaTime) const
{
	Update(InContext);
}

void FObsidianMassStateTreeEvaluator_Enemy::Update(FStateTreeExecutionContext& InContext) const
{
	const FObsidianMassCombatTargetFragment& CombatTarget = InContext.GetExternalData(CombatTargetHandle);
	const FObsidianMassEnemyStateFragment& EnemyState = InContext.GetExternalData(EnemyStateHandle);
	FInstanceDataType& InstanceData = InContext.GetInstanceData(*this);

	InstanceData.bHasTarget = CombatTarget.HasTarget();
	InstanceData.DistanceToTarget = CombatTarget.DistanceToTarget;
	InstanceData.bHitReacting = EnemyState.bHitReacting;
	InstanceData.bDying = EnemyState.bDying;

	InstanceData.TargetLocation.Reset();
	if(InstanceData.bHasTarget)
	{
		InstanceData.TargetLocation.EndOfPathPosition = CombatTarget.TargetLocation;
	}
}
