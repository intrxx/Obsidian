// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"
#include "MassStateTreeTypes.h"

#include "ObsidianMassStateTreeTask_Wait.generated.h"

struct FMassMoveTargetFragment;
struct FMassMovementParameters;
struct FMassNavMeshShortPathFragment;
struct FTransformFragment;

class UMassSignalSubsystem;

/**
 *
 */
USTRUCT()
struct FObsidianMassStateTreeTask_WaitInstanceData
{
	GENERATED_BODY()

	/** Time to wait, the Task runs until a transition stops it if this is 0. */
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0", ForceUnits = "s"), Category = "Parameter")
	float Duration = 0.0f;

	/** Adds random range to the Duration. */
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0", ForceUnits = "s"), Category = "Parameter")
	float RandomDeviation = 0.0f;

	UPROPERTY()
	float TimeLeft = 0.0f;
};

/**
 * Task that waits for the given time, the same way the Wait Task of the Behavior Trees did. State Trees of the Mass
 * entities only run when they are signaled, so the Task schedules the signal that ends it.
 */
USTRUCT(meta = (DisplayName = "Wait (Mass)", Category = "Obsidian"))
struct OBSIDIAN_API FObsidianMassStateTreeTask_Wait : public FMassStateTreeTaskBase
{
	GENERATED_BODY()

	using FInstanceDataType = FObsidianMassStateTreeTask_WaitInstanceData;

public:
	//~ Begin FStateTreeNodeBase Interface
	virtual bool Link(FStateTreeLinker& InLinker) override;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	//~ End FStateTreeNodeBase Interface

	//~ Begin FStateTreeTaskBase Interface
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& InContext, const FStateTreeTransitionResult& InTransition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& InContext, const float InDeltaTime) const override;
	//~ End FStateTreeTaskBase Interface

	/** Should the entity stop moving for the time of the wait, movement started by the other Tasks of the State is left alone if not. */
	UPROPERTY(EditAnywhere, Category = "Obsidian")
	bool bStopMovement = true;

private:
	TStateTreeExternalDataHandle<UMassSignalSubsystem> SignalSubsystemHandle;
	TStateTreeExternalDataHandle<FMassMoveTargetFragment> MoveTargetHandle;
	TStateTreeExternalDataHandle<FMassNavMeshShortPathFragment> ShortPathHandle;
	TStateTreeExternalDataHandle<FMassMovementParameters> MovementParamsHandle;
	TStateTreeExternalDataHandle<FTransformFragment> TransformHandle;
};
