// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"
#include "MassNavigationTypes.h"
#include "MassStateTreeTypes.h"

#include "ObsidianMassStateTreeTask_FindLocationAroundTarget.generated.h"

struct FObsidianMassCombatTargetFragment;

/**
 *
 */
USTRUCT()
struct FObsidianMassStateTreeTask_FindLocationAroundTargetInstanceData
{
	GENERATED_BODY()

	/** Found location in the form the navigation Tasks take, bind the Task that moves the entity to it. */
	UPROPERTY(EditAnywhere, Category = "Output")
	FMassTargetLocation LocationAroundTarget;
};

/**
 * Task for finding a random navigable location around the Target of the Enemy entity, it is found once, when the
 * State is entered. Runs in the background, so it never completes the State.
 */
USTRUCT(meta = (DisplayName = "Find Location Around Target (Mass)", Category = "Obsidian"))
struct OBSIDIAN_API FObsidianMassStateTreeTask_FindLocationAroundTarget : public FMassStateTreeTaskBase
{
	GENERATED_BODY()

	using FInstanceDataType = FObsidianMassStateTreeTask_FindLocationAroundTargetInstanceData;

public:
	FObsidianMassStateTreeTask_FindLocationAroundTarget();

	//~ Begin FStateTreeNodeBase Interface
	virtual bool Link(FStateTreeLinker& InLinker) override;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	//~ End FStateTreeNodeBase Interface

	//~ Begin FStateTreeTaskBase Interface
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& InContext, const FStateTreeTransitionResult& InTransition) const override;
	//~ End FStateTreeTaskBase Interface

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0", ForceUnits = "cm"), Category = "Obsidian")
	float RadiusAroundTheTarget = 300.0f;

	/** Location of the Target itself is used if no navigable location was found around it, the Task fails otherwise. */
	UPROPERTY(EditAnywhere, Category = "Obsidian")
	bool bFallBackToTargetIfFailed = false;

private:
	TStateTreeExternalDataHandle<FObsidianMassCombatTargetFragment> CombatTargetHandle;
};
