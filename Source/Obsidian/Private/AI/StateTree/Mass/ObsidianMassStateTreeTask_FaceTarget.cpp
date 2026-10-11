// Copyright 2026 out of sCope team - intrxx

#include "AI/StateTree/Mass/ObsidianMassStateTreeTask_FaceTarget.h"

#include "MassCommonFragments.h"
#include "MassMovementFragments.h"
#include "MassNavigationFragments.h"
#include "MassNavMeshNavigationFragments.h"
#include "MassNavMeshNavigationUtils.h"
#include "MassSignalSubsystem.h"
#include "MassStateTreeExecutionContext.h"
#include "StateTreeLinker.h"

#include "Mass/ObsidianMassEnemyFragments.h"


bool FObsidianMassStateTreeTask_FaceTarget::Link(FStateTreeLinker& InLinker)
{
	InLinker.LinkExternalData(SignalSubsystemHandle);
	InLinker.LinkExternalData(MoveTargetHandle);
	InLinker.LinkExternalData(ShortPathHandle);
	InLinker.LinkExternalData(MovementParamsHandle);
	InLinker.LinkExternalData(TransformHandle);
	InLinker.LinkExternalData(CombatTargetHandle);
	return true;
}

EStateTreeRunStatus FObsidianMassStateTreeTask_FaceTarget::EnterState(FStateTreeExecutionContext& InContext, const FStateTreeTransitionResult& InTransition) const
{
	const FMassStateTreeExecutionContext& MassContext = static_cast<FMassStateTreeExecutionContext&>(InContext);

	if(InContext.GetExternalData(CombatTargetHandle).HasTarget() == false)
	{
		return EStateTreeRunStatus::Failed;
	}

	// The entity rotates towards the forward direction of its Move Target while it stands.
	const FMassMovementParameters& MovementParams = InContext.GetExternalData(MovementParamsHandle);
	FMassNavMeshShortPathFragment& ShortPath = InContext.GetExternalData(ShortPathHandle);
	FMassMoveTargetFragment& MoveTarget = InContext.GetExternalData(MoveTargetHandle);
	MoveTarget.Center = InContext.GetExternalData(TransformHandle).GetTransform().GetLocation();
	MoveTarget.CreateNewAction(EMassMovementAction::Stand, *InContext.GetWorld());
	if(UE::MassNavigation::ActivateActionStand(InContext.GetOwner(), MassContext.GetEntity(), MovementParams.DefaultDesiredSpeed, MoveTarget, ShortPath) == false)
	{
		return EStateTreeRunStatus::Failed;
	}

	if(UpdateFacing(InContext))
	{
		return EStateTreeRunStatus::Succeeded;
	}

	InContext.GetExternalData(SignalSubsystemHandle).DelaySignalEntityDeferred(MassContext.GetMassEntityExecutionContext(),
		UE::Mass::Signals::NewStateTreeTaskRequired, MassContext.GetEntity(), CheckInterval);
	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FObsidianMassStateTreeTask_FaceTarget::Tick(FStateTreeExecutionContext& InContext, const float InDeltaTime) const
{
	const FMassStateTreeExecutionContext& MassContext = static_cast<FMassStateTreeExecutionContext&>(InContext);

	if(InContext.GetExternalData(CombatTargetHandle).HasTarget() == false)
	{
		return EStateTreeRunStatus::Failed;
	}

	if(UpdateFacing(InContext))
	{
		return EStateTreeRunStatus::Succeeded;
	}

	InContext.GetExternalData(SignalSubsystemHandle).DelaySignalEntityDeferred(MassContext.GetMassEntityExecutionContext(),
		UE::Mass::Signals::NewStateTreeTaskRequired, MassContext.GetEntity(), CheckInterval);
	return EStateTreeRunStatus::Running;
}

bool FObsidianMassStateTreeTask_FaceTarget::UpdateFacing(FStateTreeExecutionContext& InContext) const
{
	const FTransform& Transform = InContext.GetExternalData(TransformHandle).GetTransform();
	const FObsidianMassCombatTargetFragment& CombatTarget = InContext.GetExternalData(CombatTargetHandle);
	FMassMoveTargetFragment& MoveTarget = InContext.GetExternalData(MoveTargetHandle);

	const FVector ToTarget = (CombatTarget.TargetLocation - Transform.GetLocation()).GetSafeNormal2D();
	if(ToTarget.IsNearlyZero())
	{
		return true;
	}

	MoveTarget.Forward = ToTarget;

	const FVector Forward = Transform.GetRotation().GetForwardVector().GetSafeNormal2D();
	return FVector::DotProduct(Forward, ToTarget) >= FMath::Cos(FMath::DegreesToRadians(InContext.GetInstanceData(*this).Precision));
}
