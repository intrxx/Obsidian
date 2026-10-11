// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"
#include "MassStateTreeTypes.h"

#include "ObsidianMassStateTreeTask_FaceTarget.generated.h"

struct FMassMoveTargetFragment;
struct FMassMovementParameters;
struct FMassNavMeshShortPathFragment;
struct FObsidianMassCombatTargetFragment;
struct FTransformFragment;

class UMassSignalSubsystem;

/**
 *
 */
USTRUCT()
struct FObsidianMassStateTreeTask_FaceTargetInstanceData
{
	GENERATED_BODY()

	/** Success condition precision in degrees. */
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0"), Category = "Parameter")
	float Precision = 10.0f;
};

/**
 * Task for rotating the Enemy entity to face its Target, done by making it stand with the Target as its forward
 * direction. Succeeds once the entity faces the Target with given Precision, fails if there is no Target.
 */
USTRUCT(meta = (DisplayName = "Face Target (Mass)", Category = "Obsidian"))
struct OBSIDIAN_API FObsidianMassStateTreeTask_FaceTarget : public FMassStateTreeTaskBase
{
	GENERATED_BODY()

	using FInstanceDataType = FObsidianMassStateTreeTask_FaceTargetInstanceData;

public:
	//~ Begin FStateTreeNodeBase Interface
	virtual bool Link(FStateTreeLinker& InLinker) override;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	//~ End FStateTreeNodeBase Interface

	//~ Begin FStateTreeTaskBase Interface
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& InContext, const FStateTreeTransitionResult& InTransition) const override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& InContext, const float InDeltaTime) const override;
	//~ End FStateTreeTaskBase Interface

	/** Time between the checks of the rotation, the State Tree of the entity only runs when it is signaled. */
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.01", ForceUnits = "s"), Category = "Obsidian")
	float CheckInterval = 0.1f;

private:
	/** Points the entity at its Target, returns true if it already faces it. */
	bool UpdateFacing(FStateTreeExecutionContext& InContext) const;

	TStateTreeExternalDataHandle<UMassSignalSubsystem> SignalSubsystemHandle;
	TStateTreeExternalDataHandle<FMassMoveTargetFragment> MoveTargetHandle;
	TStateTreeExternalDataHandle<FMassNavMeshShortPathFragment> ShortPathHandle;
	TStateTreeExternalDataHandle<FMassMovementParameters> MovementParamsHandle;
	TStateTreeExternalDataHandle<FTransformFragment> TransformHandle;
	TStateTreeExternalDataHandle<FObsidianMassCombatTargetFragment> CombatTargetHandle;
};
