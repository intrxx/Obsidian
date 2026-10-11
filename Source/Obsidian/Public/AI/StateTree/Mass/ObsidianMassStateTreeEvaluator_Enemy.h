// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"
#include "MassNavigationTypes.h"
#include "MassStateTreeTypes.h"

#include "ObsidianMassStateTreeEvaluator_Enemy.generated.h"

struct FObsidianMassCombatTargetFragment;
struct FObsidianMassEnemyStateFragment;

/**
 *
 */
USTRUCT()
struct FObsidianMassStateTreeEvaluator_EnemyInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Output")
	bool bHasTarget = false;

	/** Distance to the Target, huge when there is no Target. */
	UPROPERTY(EditAnywhere, Category = "Output")
	float DistanceToTarget = 0.0f;

	/** Location of the Target in the form the navigation Tasks take. */
	UPROPERTY(EditAnywhere, Category = "Output")
	FMassTargetLocation TargetLocation;

	UPROPERTY(EditAnywhere, Category = "Output")
	bool bHitReacting = false;

	UPROPERTY(EditAnywhere, Category = "Output")
	bool bDying = false;
};

/**
 * Evaluator that exposes the combat target and the state of the Obsidian Enemy entity, so the conditions and the
 * Tasks of its State Tree can bind to them. This is what the Blackboard was for the Behavior Trees.
 */
USTRUCT(meta = (DisplayName = "Obsidian Enemy", Category = "Obsidian"))
struct OBSIDIAN_API FObsidianMassStateTreeEvaluator_Enemy : public FMassStateTreeEvaluatorBase
{
	GENERATED_BODY()

	using FInstanceDataType = FObsidianMassStateTreeEvaluator_EnemyInstanceData;

public:
	//~ Begin FStateTreeNodeBase Interface
	virtual bool Link(FStateTreeLinker& InLinker) override;
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	//~ End FStateTreeNodeBase Interface

	//~ Begin FStateTreeEvaluatorBase Interface
	virtual void TreeStart(FStateTreeExecutionContext& InContext) const override;
	virtual void Tick(FStateTreeExecutionContext& InContext, const float InDeltaTime) const override;
	//~ End FStateTreeEvaluatorBase Interface

private:
	void Update(FStateTreeExecutionContext& InContext) const;

	TStateTreeExternalDataHandle<FObsidianMassCombatTargetFragment> CombatTargetHandle;
	TStateTreeExternalDataHandle<FObsidianMassEnemyStateFragment> EnemyStateHandle;
};
