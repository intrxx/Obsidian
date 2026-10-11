// Copyright 2026 out of sCope team - intrxx

#pragma once

#include "CoreMinimal.h"
#include "StateTreeEvaluatorBase.h"

#include "ObsidianStateTreeEvaluator_DistanceToTarget.generated.h"

/**
 *
 */
USTRUCT()
struct FObsidianStateTreeEvaluator_DistanceToTargetInstanceData
{
	GENERATED_BODY()

	/** Actor the distance is calculated from. */
	UPROPERTY(EditAnywhere, Category = "Context")
	TObjectPtr<AActor> Actor = nullptr;

	/** Target Actor to calculate the distance to. */
	UPROPERTY(EditAnywhere, Category = "Input", meta = (Optional))
	TObjectPtr<AActor> TargetActor = nullptr;

	/** Distance to the Target Actor, it is 0 when there is no Target. */
	UPROPERTY(EditAnywhere, Category = "Output")
	float DistanceToTarget = 0.0f;

	UPROPERTY()
	float TimeToNextUpdate = 0.0f;
};

/**
 * Evaluator that calculates the distance from the State Tree Owner to the Target Actor once per Interval.
 */
USTRUCT(meta = (DisplayName = "Calculate Distance To Target", Category = "Obsidian"))
struct OBSIDIAN_API FObsidianStateTreeEvaluator_DistanceToTarget : public FStateTreeEvaluatorCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FObsidianStateTreeEvaluator_DistanceToTargetInstanceData;

public:
	//~ Begin FStateTreeNodeBase Interface
	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }
	//~ End FStateTreeNodeBase Interface

	//~ Begin FStateTreeEvaluatorBase Interface
	virtual void TreeStart(FStateTreeExecutionContext& InContext) const override;
	virtual void Tick(FStateTreeExecutionContext& InContext, const float InDeltaTime) const override;
	//~ End FStateTreeEvaluatorBase Interface

	/** Time between the updates of the distance. */
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0", ForceUnits = "s"), Category = "Obsidian")
	float Interval = 0.3f;

	/** Adds random range to the Interval. */
	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0", ForceUnits = "s"), Category = "Obsidian")
	float RandomDeviation = 0.0f;

private:
	void UpdateDistance(FInstanceDataType& InOutInstanceData) const;
};
