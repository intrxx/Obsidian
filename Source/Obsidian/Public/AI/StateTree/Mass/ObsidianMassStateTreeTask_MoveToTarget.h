// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"
#include "Tasks/MassNavMeshPathfollowTask.h"

#include "ObsidianMassStateTreeTask_MoveToTarget.generated.h"

/**
 *
 */
USTRUCT()
struct FObsidianMassStateTreeTask_MoveToTargetInstanceData : public FMassNavMeshPathFollowTaskInstanceData
{
	GENERATED_BODY()

	/** New path is requested once the Target Location gets this far from the one the current path leads to. */
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0", ForceUnits = "cm"), Category = "Parameter")
	float RepathDistance = 100.0f;

	UPROPERTY()
	FVector PathGoal = FVector::ZeroVector;
};

/**
 * NavMesh Path Follow Task that keeps up with a moving Target Location, the same way the Move To Task of the
 * Behavior Trees tracked the Actor it moved to. The base Task only finds the path once.
 */
USTRUCT(meta = (DisplayName = "Move To Target (Mass)", Category = "Obsidian"))
struct OBSIDIAN_API FObsidianMassStateTreeTask_MoveToTarget : public FMassNavMeshPathFollowTask
{
	GENERATED_BODY()

	using FInstanceDataType = FObsidianMassStateTreeTask_MoveToTargetInstanceData;

protected:
	//~ Begin FStateTreeNodeBase Interface
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	//~ End FStateTreeNodeBase Interface

	//~ Begin FStateTreeTaskBase Interface
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& InContext, const FStateTreeTransitionResult& InTransition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& InContext, const float InDeltaTime) const override;
	//~ End FStateTreeTaskBase Interface
};
